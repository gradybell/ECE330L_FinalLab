#include "battleship.h"

	//////////////////////////////////
	////////////// Data //////////////
	//////////////////////////////////

static int timer = 0; // used to control PWM

/* Input variables */
int actionButton_in = 0; // push button 10 (PC10) used to execute action (place boat & fire)
int toggleButton_in = 0; // push button 11 (PC11) used to toggle cursor between horizontal (0) and vertical (1)
int boatToggle_in = 0; // switch 8 (PC0) used to switch from 1 space (0) to 2 space (1) ships when placing
int potHorizontal_in = 0; // potentiometer (PA1) used to control horizontal cursor movement
int potVertical_in = 0; // potentiometer (PA2) used to control vertical cursor movement

/* Logic variables */
char playerOneShipsPlaced = 0; // track when player one finishes placing ships
char playerTwoShipsPlaced = 0; // track when player two finishes placing ships
char fire = 0; // detect when player has made their move
char playAgain = 0; // detect if player wants to play again

/* Output variables */
char displayToggle = 1; // toggle on (1) and off (0) to display message

/* Enumerated variables */
typedef enum gameState {title = 0, playerOneStart = 1, playerTwoStart = 2,
						playerOneTurn = 3, playerTwoTurn = 4, endGame = 5} gameState; // represents all states the game can be in
						gameState currGameState = title; // start the game at the title sequence

typedef enum cursorOrient {horizontalCursor = 0, verticalCursor = 1} cursorOrient;
				   	       cursorOrient currCursorOrient = horizontalCursor; // start the cursor in a horizontal orientation

typedef enum LEDstate {off = 0, dim = 1, bright = 2, blink = 3} LEDstate;
					   LEDstate currCursor = blink; // the cursor will always blink

/* Data structures */
/**
 * Bitmaps are composed of horizontal and vertical components and track hits and misses on the board
 */
typedef struct {
	LEDstate horizontal_bitMap[3][8]; // 2D array of integers corresponding to the horizontal LEDs on the 7SEG display
	LEDstate vertical_bitMap[2][16]; // 2D array of integers corresponding to the vertical LEDs on the 7SEG display
} bitMap;

/**
 * Players have their own map that they place boats on & an opponent map that they try to sink the ships on
 */
typedef struct {
	bitMap *ownMap; // points to board with their own ships
	bitMap *opponentMap; // points to board with their opponent's ships
	char doubleBoatProperties[2][3]; // track details about the two double boats
									 /* Boat 1 [0][i] */
										 //	i=0: horizontal (0) or vertical (1)
										 //	i=1: row
										 //	i=2: col
									 /* Boat 2 [1][i] */
										 //	i=0: horizontal (0) or vertical (1)
										 //	i=1: row
										 //	i=2: col
	char singleBoatsRemaining; // track number of remaining single-spaced boats
	char doubleBoatsRemaining; // track number of remaining double-spaced boats
	char hits; // track total number of hits
	char misses; // track total number of misses	 //TODO delete if unnecessary
} player;

/* Instances of maps */
// Player One
bitMap playerOneShips = {
	.horizontal_bitMap = {
			{ off, off, off, dim, dim, off, off, off },
			{ off, off, off, bright, bright, off, off, off },
			{ off, off, off, blink, blink, off, off, off },
	},
//	{ off, off, off, dim, dim, off, off, off },
//	{ off, off, off, bright, bright, off, off, off },
//	{ off, off, off, blink, blink, off, off, off },

	.vertical_bitMap = { 0 }
};
// Player Two
bitMap playerTwoShips = {
	.horizontal_bitMap = { 0 },
	.vertical_bitMap = { 0 }
};

/* Instance of players 1 */
// Player One
player playerOne = {
		.ownMap = &playerOneShips,
		.opponentMap = &playerTwoShips,
		.singleBoatsRemaining = 3,
		.doubleBoatsRemaining = 2,
		.hits = 0,
		.misses = 0
};
// Player Two
player playerTwo = {
		.ownMap = &playerTwoShips,
		.opponentMap = &playerOneShips,
		.singleBoatsRemaining = 3,
		.doubleBoatsRemaining = 2,
		.hits = 0,
		.misses = 0
};

	///////////////////////////////////
	////// Function Declarations //////
	///////////////////////////////////

bitMap cursorPosition(int hPot, int vPot, enum cursorOrient orient);
bitMap compileBoard(bitMap *cursor, bitMap *map);
void assignIndex_State(char row, char col, bitMap *map, cursorOrient orient, LEDstate state);
void drawBoard(bitMap *map);

	//////////////////////////////////
	////// Function Definitions //////
	//////////////////////////////////

/**
 * Process inputs from push buttons and potentiometers
 */
int input(void) {
	actionButton_in = (GPIOC->IDR >> 10) & 1; // parse only the switch connected to PC10
	toggleButton_in = (GPIOC->IDR >> 11) & 1; // parse only the switch connected to PC11
	boatToggle_in = GPIOC->IDR & 1; // parse only PC0

	/* set orientation of cursor */
	if (toggleButton_in == 0) {
		if (currCursorOrient == verticalCursor) { // the cursor is horizontal
			currCursorOrient = horizontalCursor;
			HAL_Delay(300);
		} else {
			currCursorOrient = verticalCursor;
			HAL_Delay(300);
		}
	}

	/* read potentiometers */
		// Horizontal
		ADC1->SQR3 = 1; // select channel 1
		ADC1->CR2 |= 1<<30; // start sequence of reading & converting channel 1
		HAL_Delay(40);
		potHorizontal_in = ADC1->DR; // read value from potentiometer at PA1

		// Vertical
		ADC1->SQR3 = 2; // select channel 2
		ADC1->CR2 |= 1<<30; // start sequence of reading & converting channel 2
		HAL_Delay(40);
		potVertical_in = ADC1->DR; // read value from potentiometer at PA2

	return 0;
}

/**
 * Game logic that reads inputs and produces outputs
 */
int logic (void) {

	// process raw inputs

	// game logic
	switch (currGameState) {

		case title:
			currGameState = playerOneStart;
			break;

		case playerOneStart:
			while (playerOneShipsPlaced & 0) {
//TODO logic
			}
			playerOneShipsPlaced = 1;
			currGameState = playerTwoStart;
			break;

		case playerTwoStart:
			while (playerTwoShipsPlaced & 0) {
//TODO logic
			}
			playerTwoShipsPlaced = 1;
			currGameState = playerOneTurn;
			break;

		case playerOneTurn:
			while (fire & 0) { // loop until player moves
//TODO logic
			}
			if (playerOne->hits == 7) {
				currGameState = endGame; // game ends
				break;
			}
			currGameState = playerTwoTurn; // toggle  to player two
			break;

		case playerTwoTurn:
			while (fire & 0) { // loop until player moves
//TODO logic
			}
			if (playerTwo->hits == 7) {
				currGameState = endGame; // game ends
				break;
			}
			currGameState = playerOneTurn; // toggle to player one
			break;

		case endGame:
			if (playAgain & 1) {
//TODO logic
			}
			break;
	}

	// prepare outputs

	return 0;
}

/**
 * prepares output and draws board to 7SEG display
 */
int output (void) {

	/* (TODO delete) TESTING */
	bitMap cursorBoard = cursorPosition(potHorizontal_in, potVertical_in, currCursorOrient);
	bitMap display = compileBoard(&cursorBoard, &playerOneShips);
	drawBoard(&display);

	switch (currGameState) {

		case title:
			/* Display Game Title */
			if (displayToggle & 1) {
				displayToggle = 0;
				char MessageTitle[] = // scrolls "Battleship" on Marquee display
						{SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,
						CHAR_B, CHAR_A, CHAR_T, CHAR_T, CHAR_L, CHAR_E,
						CHAR_S, CHAR_H, CHAR_I, CHAR_P,
						SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE};
				Animate_On = 1; // turn on animation so that interrupt displays Message[]
				Message_Pointer = &MessageTitle[0];
				Save_Pointer = &MessageTitle[0];
				Message_Length = sizeof(MessageTitle)/sizeof(MessageTitle[0]);
				Delay_msec = 200;
				HAL_Delay(5000);           // Delay 5 seconds to allow message to scroll
				Animate_On = 0;            // Stop scrolling message
				HAL_Delay(1000);           // Delay 1 second
			}

			break;

		case playerOneStart:
			/* Display Player One Start Screen*/
			char MessageP1Start[] = // scrolls "Player One - Place Ships" on Marquee display
					{SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,
					CHAR_P, CHAR_L, CHAR_A, CHAR_Y, CHAR_E, CHAR_R, SPACE,
					CHAR_O, CHAR_N, CHAR_E, SPACE, DASH, SPACE,
					CHAR_P, CHAR_L, CHAR_A, CHAR_C, CHAR_E, SPACE,
					CHAR_S, CHAR_H, CHAR_I, CHAR_P, CHAR_S,
					SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE};
			Animate_On = 1; // turn on animation so that interrupt displays Message[]
			Message_Pointer = &MessageP1Start[0];
			Save_Pointer = &MessageP1Start[0];
			Message_Length = sizeof(MessageP1Start)/sizeof(MessageP1Start[0]);
			Delay_msec = 200;
			HAL_Delay(10000);          // Delay 10 seconds to allow message to scroll
			Animate_On = 0;            // Stop scrolling message
			HAL_Delay(1000);           // Delay 1 second
			break;

		case playerTwoStart:
			/* Display Player Two Start Screen*/
			char MessageP2Start[] = // scrolls "Player Two - Place Ships" on Marquee display
					{SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,
					CHAR_P, CHAR_L, CHAR_A, CHAR_Y, CHAR_E, CHAR_R, SPACE,
					CHAR_T, CHAR_W, CHAR_O, SPACE, DASH, SPACE,
					CHAR_P, CHAR_L, CHAR_A, CHAR_C, CHAR_E, SPACE,
					CHAR_S, CHAR_H, CHAR_I, CHAR_P, CHAR_S,
					SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE};
			Animate_On = 1; // turn on animation so that interrupt displays Message[]
			Message_Pointer = &MessageP2Start[0];
			Save_Pointer = &MessageP2Start[0];
			Message_Length = sizeof(MessageP2Start)/sizeof(MessageP2Start[0]);
			Delay_msec = 200;
			HAL_Delay(10000);          // Delay 10 seconds to allow message to scroll
			Animate_On = 0;            // Stop scrolling message
			HAL_Delay(1000);           // Delay 1 second
			break;

		case playerOneTurn:
			break;

		case playerTwoTurn:
			break;

		/**
		 * prompt players to restart the game and preserve win record with points? If we have the time
		 */
		case endGame:
			/* Display Game End Screen*/
			if (playerOne->hits == 7) { // player one won
				char MessageP1Wins[] = // scrolls "Player Two - Place Ships" on Marquee display
						{SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,
						CHAR_P, CHAR_L, CHAR_A, CHAR_Y, CHAR_E, CHAR_R, SPACE,
						CHAR_O, CHAR_N, CHAR_E, SPACE,
						CHAR_W, CHAR_I, CHAR_N, CHAR_S,
						SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE};
				Animate_On = 1; // turn on animation so that interrupt displays Message[]
				Message_Pointer = &MessageP1Wins[0];
				Save_Pointer = &MessageP1Wins[0];
				Message_Length = sizeof(MessageP1Wins)/sizeof(MessageP1Wins[0]);
				Delay_msec = 200;
				HAL_Delay(10000);          // Delay 10 seconds to allow message to scroll
				Animate_On = 0;            // Stop scrolling message
				HAL_Delay(1000);           // Delay 1 second
			} else { // player two won
				char MessageP2Wins[] = // scrolls "Player Two - Place Ships" on Marquee display
						{SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,
						CHAR_P, CHAR_L, CHAR_A, CHAR_Y, CHAR_E, CHAR_R, SPACE,
						CHAR_T, CHAR_W, CHAR_O, SPACE,
						CHAR_W, CHAR_I, CHAR_N, CHAR_S,
						SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE};
				Animate_On = 1; // turn on animation so that interrupt displays Message[]
				Message_Pointer = &MessageP2Wins[0];
				Save_Pointer = &MessageP2Wins[0];
				Message_Length = sizeof(MessageP2Wins)/sizeof(MessageP2Wins[0]);
				Delay_msec = 200;
				HAL_Delay(10000);          // Delay 10 seconds to allow message to scroll
				Animate_On = 0;            // Stop scrolling message
				HAL_Delay(1000);           // Delay 1 second
			}
			break;
	}

	return 0;
}

int handle_interrupts (void) {

	timer++;

	return 0;
}

int game(void) {

	while(1) {
		  input();
		  logic();
		  output();
	}
}

	/////////////////////////////////
	//////////// Helpers ////////////
	/////////////////////////////////
/**
 * This method reads potentiometer values to set the cursor position.
 * It does this by checking the orientation of the cursor, then
 * moving through conditional checks to allow even ranges for the
 * potentiometers to sweep through for given positions
 */
bitMap cursorPosition(int hPot, int vPot, enum cursorOrient orient) {
	bitMap cursorBoard = {
			.horizontal_bitMap = { 0 },
			.vertical_bitMap = { 0 }
	};
	char col, row;

	/* determine column & row */
	if (orient == horizontalCursor) { // horizontal segments
	//horizontal movement
		if (hPot < 512) { // leftmost position = leftmost segment
			col = 0;
		} else if (hPot < 1024) {
			col = 1;
		} else if (hPot < 1536) {
			col = 2;
		} else if (hPot < 2048) { // middle position = middle segment
			col = 3;
		} else if (hPot < 2560) {
			col = 4;
		} else if (hPot < 3072) {
			col = 5;
		} else if (hPot < 3584) {
			col = 6;
		} else { // rightmost position = rightmost segment
			col = 7;
		}
	//vertical movement
		if (vPot < 1365) { // left position = top
			row = 0;
		} else if (vPot < 2730) { // middle position = middle
			row = 1;
		} else { // right position = bottom
			row = 2;
		}

		assignIndex_State(row, col, &cursorBoard, horizontalCursor, currCursor);

	} else { // vertical segments
	//horizontal movement
		if (hPot < 256) { // leftmost position = leftmost segment
			col = 0;
		} else if (hPot < 512) {
			col = 1;
		} else if (hPot < 768) {
			col = 2;
		} else if (hPot < 1024) {
			col = 3;
		} else if (hPot < 1280) {
			col = 4;
		} else if (hPot < 1536) {
			col = 5;
		} else if (hPot < 1792) {
			col = 6;
		} else if (hPot < 2048) { // middle position = middle segment
			col = 7;
		} else if (hPot < 2304) {
			col = 8;
		} else if (hPot < 2560) {
			col = 9;
		} else if (hPot < 2816) {
			col = 10;
		} else if (hPot < 3072) {
			col = 11;
		} else if (hPot < 3328) {
			col = 12;
		} else if (hPot < 3584) {
			col = 13;
		} else if (hPot < 3840) {
			col = 14;
		} else { // rightmost position = rightmost segment
			col = 15;
		}
	//vertical movement
		if (vPot < 2048) { // left position = top
			row = 0;
		} else { // right position = bottom
			row = 1;
		}

		assignIndex_State(row, col, &cursorBoard, verticalCursor, currCursor);
	}

	return cursorBoard;
}

/**
 * set a specific location on the board to a given state
 */
void assignIndex_State(char row, char col, bitMap *map, cursorOrient orient, LEDstate state) {

	if (orient == horizontalCursor) {
		map->horizontal_bitMap[row][col] = state;
	} else {
		map->vertical_bitMap[row][col] = state;
	}
	return;
}

bitMap compileBoard(bitMap *cursor, bitMap *map) { // TODO try rewriting to just print map onto every empty index of cursor
	bitMap comp = {
			.horizontal_bitMap = { 0 },
			.vertical_bitMap = { 0 }
	};

	/* compile horizontal */
	for (int row = 0; row < 3; row++) {
		for (int col = 0; col < 8; col++) {
			if (cursor->horizontal_bitMap[row][col] & 1) {
				comp.horizontal_bitMap[row][col] = cursor->horizontal_bitMap[row][col];
			} else {
				comp.horizontal_bitMap[row][col] = map->horizontal_bitMap[row][col];
			}
		}
	}
	/* compile vertical */
	for (int row = 0; row < 2; row++) {
		for (int col = 0; col < 16; col++) {
			if (cursor->vertical_bitMap[row][col] & 1) {
				comp.vertical_bitMap[row][col] = cursor->vertical_bitMap[row][col];
			} else {
				comp.vertical_bitMap[row][col] = map->vertical_bitMap[row][col];
			}
		}
	}

	return comp;
}
void drawBoard(bitMap *map) {

	char seg[8] = { 0 };

	/* set the horizontal segments */
	for (int i = 0; i < 8; i++)	{

		// set the top segments
		switch(map->horizontal_bitMap[0][i]) {
			case off:
				break;
			case dim:
				if (timer % 3 == 0) // TODO revisit to get dim
					seg[i] |= 1;
				break;
			case bright:
				seg[i] |= 1;
				break;
			case blink:
				if (timer % 6 == 0)
					seg[i] |= 1;
				break;
		}

		// set the middle segments
		switch(map->horizontal_bitMap[1][i]) {
			case off:
				break;
			case dim:
				if (timer % 3 == 0) // TODO revisit to get dim
					seg[i] |= 1<<6;
				break;
			case bright:
				seg[i] |= 1<<6;
				break;
			case blink:
				if (timer % 6 == 0)
					seg[i] |= 1<<6;
				break;
		}

		// set the bottom segments
		switch(map->horizontal_bitMap[2][i]) {
			case off:
				break;
			case dim:
				if (timer % 3 == 0) // TODO revisit to get dim
					seg[i] |= 1<<3;
				break;
			case bright:
				seg[i] |= 1<<3;
				break;
			case blink:
				if (timer % 6 == 0)
					seg[i] |= 1<<3;
				break;
		}
	}

	/* set vertical segments */
	/**
	 * Iterate through each column of the vertical bitmap array and
	 * set the appropriate bits at seg[i/2] so that the 16 bit vertical
	 * array is mapped to the 8 bit character array, for which each
	 * character contains the information of both the left and right
	 * vertical segments
	 */
	for (int i = 0; i < 16; i++) {
		/* left */
		// set the top left segments
		switch(map->vertical_bitMap[0][i]) {
			case off:
				break;
			case dim:
				if (timer % 3 == 0) // TODO revisit to get dim
					seg[i/2] |= 1<<5;
				break;
			case bright:
				seg[i/2] |= 1<<5;
				break;
			case blink:
				if (timer % 6 == 0)
					seg[i/2] |= 1<<5;
				break;
		}

		// set the bottom left segments
		switch(map->vertical_bitMap[1][i]) {
			case off:
				break;
			case dim:
				if (timer % 3 == 0) // TODO revisit to get dim
					seg[i/2] |= 1<<4;
				break;
			case bright:
				seg[i/2] |= 1<<4;
				break;
			case blink:
				if (timer % 6 == 0)
					seg[i/2] |= 1<<4;
				break;
		}

		/* right */
		i++; // move to next index of vertical array
		// set the top right segments
		switch(map->vertical_bitMap[0][i]) {
			case off:
				break;
			case dim:
				if (timer % 3 == 0) // TODO revisit to get dim
					seg[i/2] |= 1<<1;
				break;
			case bright:
				seg[i/2] |= 1<<1;
				break;
			case blink:
				if (timer % 6 == 0)
					seg[i/2] |= 1<<1;
				break;
		}

		// set the bottom right segments
		switch(map->vertical_bitMap[1][i]) {
			case off:
				break;
			case dim:
				if (timer % 3 == 0) // TODO revisit to get dim
					seg[i/2] |= 1<<2;
				break;
			case bright:
				seg[i/2] |= 1<<2;
				break;
			case blink:
				if (timer % 6 == 0)
					seg[i/2] |= 1<<2;
				break;
		}
	}

	for (char display = 0; display < 8; display++) {
		GPIOE->ODR = ((0xFF00 | ~seg[7-display]) & ~(1<<(display+8))) | 0x80;
	}

	// Set all selects high to latch-in character
	HAL_Delay(2);
	GPIOE->ODR |= 0xFF00;
	return;
}
