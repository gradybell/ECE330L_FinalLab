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

typedef enum player {playerOne = 0, playerTwo = 1} player;

typedef enum LEDstate {off = 0, dim = 1, bright = 2, blink = 3} LEDstate;

static int timer = 0; // used to control game timing

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
	.hits = 0,
	.misses = 0,
	.singleBoatsRemaining = 3,
	.doubleBoatsRemaining = 2,
	.horizontal_bitMap = {
		{ off, off, bright, bright, bright, bright, off, off },
		{ off, off, bright, bright, bright, bright, off, off },
		{ off, off, bright, bright, bright, bright, off, off },
	},
	.vertical_bitMap = { 0 }
};

bitMap playerOneTargetBoard = {
	.hits = 0,
	.misses = 0,
	.singleBoatsRemaining = 3,
	.doubleBoatsRemaining = 2,
	.horizontal_bitMap = {
		{ off, off, bright, bright, bright, bright, off, off },
		{ off, off, bright, bright, bright, bright, off, off },
		{ off, off, bright, bright, bright, bright, off, off },
	},
	.vertical_bitMap = { 0 }
};

/* Player Two */
bitMap playerTwoShipBoard = {
	.hits = 0,
	.misses = 0,
	.singleBoatsRemaining = 3,
	.doubleBoatsRemaining = 2,
	.horizontal_bitMap = {
		{ off, off, bright, bright, bright, bright, off, off },
		{ off, off, bright, bright, bright, bright, off, off },
		{ off, off, bright, bright, bright, bright, off, off },
	},
	.vertical_bitMap = { 0 }
};

bitMap playerTwoTargetBoard = {
	.hits = 0,
	.misses = 0,
	.singleBoatsRemaining = 3,
	.doubleBoatsRemaining = 2,
	.horizontal_bitMap = {
		{ off, off, bright, bright, bright, bright, off, off },
		{ off, off, bright, bright, bright, bright, off, off },
		{ off, off, bright, bright, bright, bright, off, off },
	},
	.vertical_bitMap = { 0 }
};

//////////////////////////////////
////// Function Definitions //////
//////////////////////////////////

/**
 * Process inputs from switches, pushbuttons, and potentiometer
 */
int input(void) {

	switches_in = GPIOC->IDR & 1; // parse only the rightmost switch (PC0)
	pushButton_in = (GPIOC->IDR >> 10) & 1; // parse only the switch connected to PC10

	/* read potentiometer  */
		HAL_Delay(1);
		/*
		 * Start a conversion on ADC1 by forcing bit 30 in CR2 to
		 * 1 while keeping other bits unchanged
		 */
		ADC1->CR2 |= 1<<30;
		HAL_Delay(1);
		if (ADC1->SR & 1<<1) { // check for conversion completed
			potHorizontal_in = (ADC1->DR);
		}
		HAL_Delay(2500);
		if (ADC1->SR & 1<<1) { // check for conversion completed
			potVertical_in = (ADC1->DR);
		}

	return 0;
}

/**
 * Game logic that reads inputs and produces outputs
 */
int logic (void) {

	// process raw inputs

	// game logic
	switch (gameState) { // game state dependent logic

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
 * draws board to 7SEG display
 */
int output (void) {

	GPIOD->ODR = potHorizontal_in; // TODO delete this (for testing only)
	HAL_Delay(5000);
	GPIOD->ODR = potVertical_in;

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

void drawBoard(struct bitMap) {

	return 0;
}
