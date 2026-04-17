#include "battleship.h"

//////////////////////////////////
////////////// Data //////////////
//////////////////////////////////

/* Input variables */
int switches_in = 0; // slider switch (PC0) used to toggle cursor between horizontal (0) and vertical (1)
int pushButton_in = 0; // pushbutton (PC10) used to execute action (place boat & fire)
int potHorizontal_in = 0; // potentiometer (PA1) used to control horizontal cursor movement
int potVertical_in = 0; // potentiometer (PA2) used to control vertical cursor movement

/* Output variables */
/*****************************/
/*** TODO output variables ***/
/*****************************/

/* Enumerated variables */
typedef enum gameState {title = 0, playerOneStart = 1, playerTwoStart = 2,
						playerOneTurn = 3, playerTwoTurn = 4, endGame = 5} gameState; // represents all states the game can be in

						gameState currGameState = title; // TODO change back to title

typedef enum player {playerOne = 0, playerTwo = 1} player;

typedef enum LEDstate {off = 0, dim = 1, bright = 2, blink = 3} LEDstate;

static int timer = 0; // used to control PWM

/* Data structure */
typedef struct {
	LEDstate horizontal_bitMap[3][8]; // 2D array of integers corresponding to the horizontal LEDs on the 7SEG display
	LEDstate vertical_bitMap[2][16]; // 2D array of integers corresponding to the vertical LEDs on the 7SEG display
	LEDstate cursor; // blinking cursor
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
	.cursor = blink,
	.hits = 0,
	.misses = 0,
	.singleBoatsRemaining = 3,
	.doubleBoatsRemaining = 2
};

bitMap playerOneTargetBoard = {
	.horizontal_bitMap = { 0 },
	.vertical_bitMap = { 0 },
	.cursor = blink,
	.hits = 0,
	.misses = 0,
	.singleBoatsRemaining = 3,
	.doubleBoatsRemaining = 2
};

/* Player Two */
bitMap playerTwoShipBoard = {
	.horizontal_bitMap = { 0 },
	.vertical_bitMap = { 0 },
	.cursor = blink,
	.hits = 0,
	.misses = 0,
	.singleBoatsRemaining = 3,
	.doubleBoatsRemaining = 2
};

bitMap playerTwoTargetBoard = {
	.horizontal_bitMap = { 0 },
	.vertical_bitMap = { 0 },
	.cursor = blink,
	.hits = 0,
	.misses = 0,
	.singleBoatsRemaining = 3,
	.doubleBoatsRemaining = 2
};

bitMap cursorBoard = {
	.horizontal_bitMap = { 0 },
	.vertical_bitMap = { 0 },
	.cursor = blink
};

///////////////////////////////////
////// Function Declarations //////
///////////////////////////////////

void drawBoard(bitMap m);
bitMap compileBoard(bitMap cursor, bitMap map);

//////////////////////////////////
////// Function Definitions //////
//////////////////////////////////

/**
 * Process inputs from switches, pushbuttons, and potentiometers
 */
int input(void) {

	switches_in = GPIOC->IDR & 1; // parse only the rightmost switch (PC0)
	pushButton_in = (GPIOC->IDR >> 10) & 1; // parse only the switch connected to PC10

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
	switch (currGameState) { // game state dependent logic

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

	drawBoard(cursorPosition(potHorizontal_in, potVertical_in, switches_in));
//	compileBoard(cursorBoard, playerOneShipBoard);

	switch (currGameState) { // game state dependent logic

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

bitMap cursorPosition(int hPot, int vPot, int sw) {
	bitMap cursorBoard;
	cursorBoard.horizontal_bitMap = { 0 };
	cursorBoard.vertical_bitMap = { 0 };
		
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
			if (cursor.horizontal_bitMap[i][j] > 0) {
				comp.horizontal_bitMap[i][j] = cursor.horizontal_bitMap[i][j];
			} else {
				comp.horizontal_bitMap[i][j] = map.horizontal_bitMap[i][j];
			}
		}
	}
	/* compile vertical */
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 16; j++) {
			if (cursor.vertical_bitMap[i][j] > 0) {
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
				if (timer % 9 == 0)
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
				if (timer % 9 == 0)
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
				if (timer % 9 == 0)
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
				if (timer % 9 == 0)
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
				if (timer % 9 == 0)
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
				if (timer % 9 == 0)
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
				if (timer % 9 == 0)
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
