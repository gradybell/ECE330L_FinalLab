#include "battleship.h"

	//////////////////////////////////
	////////////// Data //////////////
	//////////////////////////////////

static int timer = 0; // used to control PWM

/* Input variables */
int actionButton_in = 0; // push button 10 (PC10) used to execute action (place boat & fire)
int toggleButton_in = 0; // push button 11 (PC11) used to toggle cursor between horizontal (0) and vertical (1)
int potHorizontal_in = 0; // potentiometer (PA1) used to control horizontal cursor movement
int potVertical_in = 0; // potentiometer (PA2) used to control vertical cursor movement

/* Output variables */
/*****************************/
/*** TODO output variables ***/
/*****************************/

/* Enumerated variables */
typedef enum gameState {title = 0, playerOneStart = 1, playerTwoStart = 2,
						playerOneTurn = 3, playerTwoTurn = 4, endGame = 5} gameState; // represents all states the game can be in
						gameState currGameState = title; // start the game at the title sequence

typedef enum player {playerOne = 0, playerTwo = 1} player;
					 player currPlayer = playerOne; // start with player one

typedef enum cursorOrient {horizontalCursor = 0, verticalCursor = 1} cursorOrient;
				   	       cursorOrient currCursorOrient = horizontalCursor; // start the cursor in a horizontal orientation

typedef enum LEDstate {off = 0, dim = 1, bright = 2, blink = 3} LEDstate;
					   LEDstate currCursor = blink; // the cursor will always blink

/* Data structure */
typedef struct {
	LEDstate horizontal_bitMap[3][8]; // 2D array of integers corresponding to the horizontal LEDs on the 7SEG display
	LEDstate vertical_bitMap[2][16]; // 2D array of integers corresponding to the vertical LEDs on the 7SEG display
	char hits; // track number of hits on board
	char misses; // track number of misses on board
	char singleBoatsRemaining; // track number of remaining single-spaced boats
	char doubleBoatsRemaining; // track number of remaining double-spaced boats
} bitMap;

/**
 *  Instances of maps
 */
/* Player One */
bitMap playerOneShipBoard = {
	.horizontal_bitMap = {
			{ bright, bright, dim, dim, bright, bright, off, off },
			{ off, off, bright, bright, dim, dim, bright, bright },
			{ bright, bright, off, off, bright, bright, dim, dim }
	},
	.vertical_bitMap = {
			{ off, off, off, off, bright, off, bright, bright,
			  dim, dim, bright, off, dim, off, bright, off },
			{ dim, dim, bright, off, dim, off, bright, off,
			  off, off, off, off, bright, off, bright, bright }
	},
	.hits = 0,
	.misses = 0,
	.singleBoatsRemaining = 3,
	.doubleBoatsRemaining = 2
};

bitMap playerOneTargetBoard = {
	.horizontal_bitMap = { 0 },
	.vertical_bitMap = { 0 },
	.hits = 0,
	.misses = 0,
	.singleBoatsRemaining = 3,
	.doubleBoatsRemaining = 2
};

/* Player Two */
bitMap playerTwoShipBoard = {
	.horizontal_bitMap = { 0 },
	.vertical_bitMap = { 0 },
	.hits = 0,
	.misses = 0,
	.singleBoatsRemaining = 3,
	.doubleBoatsRemaining = 2
};

bitMap playerTwoTargetBoard = {
	.horizontal_bitMap = { 0 },
	.vertical_bitMap = { 0 },
	.hits = 0,
	.misses = 0,
	.singleBoatsRemaining = 3,
	.doubleBoatsRemaining = 2
};

bitMap cursorBoard = {
	.horizontal_bitMap = { 0 },
	.vertical_bitMap = { 0 },
};

	///////////////////////////////////
	////// Function Declarations //////
	///////////////////////////////////

bitMap cursorPosition(int hPot, int vPot, enum cursorOrient orient);
bitMap compileBoard(bitMap cursor, bitMap map);
void drawBoard(bitMap m);

	//////////////////////////////////
	////// Function Definitions //////
	//////////////////////////////////

/**
 * Process inputs from push buttons and potentiometers
 */
int input(void) {
	actionButton_in = (GPIOC->IDR >> 10) & 1; // parse only the switch connected to PC10
	toggleButton_in = (GPIOC->IDR >> 11) & 1; // parse only the switch connected to PC11

	/* set orientation of cursor */
	if (!(toggleButton_in & 1)) {
		if (!(currCursorOrient & 1)) { // the cursor is horizontal
			currCursorOrient = verticalCursor;
			HAL_Delay(10);
		} else {
			currCursorOrient = horizontalCursor;
			HAL_Delay(10);
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

			break;

		case playerOneStart:

			break;

		case playerTwoStart:

			break;

		case playerOneTurn:

			break;

		case playerTwoTurn:

			break;

		case endGame:

			break;
	}

	// prepare outputs

	return 0;
}

/**
 * prepares output and draws board to 7SEG display
 */
int output (void) {

	GPIOD->ODR = toggleButton_in<<2 | actionButton_in;
	drawBoard(cursorPosition(potHorizontal_in, potVertical_in, currCursorOrient));
//	compileBoard(cursorBoard, playerOneShipBoard);

	switch (currGameState) {

		case title:

			break;

		case playerOneStart:
			drawBoard(playerOneShipBoard);
			break;

		case playerTwoStart:
			drawBoard(playerTwoShipBoard);
			break;

		case playerOneTurn:
			drawBoard(playerOneTargetBoard);
			break;

		case playerTwoTurn:
			drawBoard(playerTwoTargetBoard);
			break;

		case endGame:

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
	int col, row;

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
	}

	cursorBoard.horizontal_bitMap[row][col] = currCursor; // set cursor location

	return cursorBoard;
}

bitMap compileBoard(bitMap cursor, bitMap map) {
	bitMap comp = {
			.horizontal_bitMap = { 0 },
			.vertical_bitMap = { 0 },
			.hits = map.hits,
			.misses = map.misses,
			.singleBoatsRemaining = map.singleBoatsRemaining,
			.doubleBoatsRemaining = map.doubleBoatsRemaining,
	};

	/* compile horizontal */
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 8; j++) {
			if (cursor.horizontal_bitMap[i][j] & 1) {
				comp.horizontal_bitMap[i][j] = cursor.horizontal_bitMap[i][j];
			} else {
				comp.horizontal_bitMap[i][j] = map.horizontal_bitMap[i][j];
			}
		}
	}
	/* compile vertical */
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 16; j++) {
			if (cursor.vertical_bitMap[i][j] & 1) {
				comp.vertical_bitMap[i][j] = cursor.vertical_bitMap[i][j];
			} else {
				comp.vertical_bitMap[i][j] = map.vertical_bitMap[i][j];
			}
		}
	}

	return comp;
}
void drawBoard(bitMap m) {

	char seg[8] = { 0 };

	/* set the horizontal segments */
	for (int i = 0; i < 8; i++)	{

		// set the top segments
		switch(m.horizontal_bitMap[0][i]) {
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
		switch(m.horizontal_bitMap[1][i]) {
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
		switch(m.horizontal_bitMap[2][i]) {
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
		switch(m.vertical_bitMap[0][i]) {
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
		switch(m.vertical_bitMap[1][i]) {
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
		switch(m.vertical_bitMap[0][i]) {
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
		switch(m.vertical_bitMap[1][i]) {
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
	GPIOE->ODR |= 0xFF00;
	return;
}
