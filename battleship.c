#include "battleship.h"

//////////////////////////////////
////////////// Data //////////////
//////////////////////////////////

/* Input variables */
int switches_in = 0; // slider switch (PC0) used to toggle cursor between horizontal (0) and vertical (1)
int pushButton_in = 0; // pushbutton (PC10) used to execute action (place boat & fire)
int pot_in = 0; // potentiometer (PA1) used to control cursor

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
typedef struct bitMap {
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
bitMap playerOneShipBoard;
playerOneShipBoard.hits = 0;
playerOneShipBoard.misses = 0;
playerOneShipBoard.singleBoatsRemaining = 3;
playerOneShipBoard.doubleBoatsRemaining = 2;
bitMap playerOneTargetBoard;
playerOneTargetBoard.hits = 0;
playerOneTargetBoard.misses = 0;
playerOneTargetBoard.singleBoatsRemaining = 3;
playerOneTargetBoard.doubleBoatsRemaining = 2;

/* Player Two */
bitMap playerTargetShipBoard;
playerTargetShipBoard.hits = 0;
playerTargetShipBoard.misses = 0;
playerTargetShipBoard.singleBoatsRemaining = 3;
playerTargetShipBoard.doubleBoatsRemaining = 2;
bitMap playerTargetTargetBoard;
playerTargetTargetBoard.hits = 0;
playerTargetTargetBoard.misses = 0;
playerTargetTargetBoard.singleBoatsRemaining = 3;
playerTargetTargetBoard.doubleBoatsRemaining = 2;

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
			pot_in = (ADC1->DR);
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

	GPIOD->ODR = pot_in; // TODO delete this (for testing only)

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

}
