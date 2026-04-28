#include "battleship.h"

	//////////////////////////////////
	////////////// Data //////////////
	//////////////////////////////////

static int timer = 0; // used to control PWM
static int cursorPosition[2]; // track the location of the cursor
							  // index 0: row
							  // index 1: col

/*** Delay Variables to be used in HAL_Delay ***/
extern int pushButton_delay = 300;
extern int displayHold_delay = 1000;
extern int title_delay = 5000;
extern int recap_delay = 8000;
extern int playerStart_delay = 10000;
extern int round_delay = 2000;
extern int extRound_delay = 2100;
extern int move_delay = 1800;
extern int winner_delay = 5000;
extern int endGame_delay = 10000;

/*** Input variables ***/
int actionButton_in = 0; // push button 10 (PC10) used to execute action (place boat & fire)
int toggleButton_in = 0; // push button 11 (PC11) used to toggle cursor between horizontal (0) and vertical (1)
int shipTypeSwitch_in = 0; // switch 8 (PC0) used to switch from 1 space (0) to 2 space (1) ships when placing
int potHorizontal_in = 0; // potentiometer (PA1) used to control horizontal cursor movement
int potVertical_in = 0; // potentiometer (PA2) used to control vertical cursor movement

/*** Logic variables ***/
bool playerOneShipsPlaced = false; // track when player one finishes placing ships
bool playerTwoShipsPlaced = false; // track when player two finishes placing ships
char numRounds = 0; // track number of games played
char playAgain = 0; // detect if player wants to play again
bool firstGame = true; // detect if this is the first game played

/*** Output variables ***/
char displayToggle = 1; // toggle on (1) and off (0) to display message
char displayRound_0th = 0x0; // the 0th position of the number of rounds played
char displayRound_1st = 0x0; // the 1st position of the number of rounds played

	/* Marquee Messages */
char MessageTitle[] = // scrolls "Battleship" on Marquee display
	{SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,
	 CHAR_B, CHAR_A, CHAR_T, CHAR_T, CHAR_L, CHAR_E,
	 CHAR_S, CHAR_H, CHAR_I, CHAR_P,
	 SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE};

char MessageP1Recap[] = // scrolls "Player 1 - _ Wins" on Marquee display
	{SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,
	 CHAR_P, CHAR_L, CHAR_A, CHAR_Y, CHAR_E, CHAR_R, SPACE,
	 CHAR_1, SPACE, DASH, SPACE,
	 UNDER, SPACE, CHAR_W, CHAR_I, CHAR_N, CHAR_S, // UNDER (index 21) is replaced with number of P1 wins
	 SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE};
char MessageP2Recap[] = // scrolls "Player 2 - _ Wins" on Marquee display
	{SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,
	 CHAR_P, CHAR_L, CHAR_A, CHAR_Y, CHAR_E, CHAR_R, SPACE,
	 CHAR_2, SPACE, DASH, SPACE,
	 UNDER, SPACE, CHAR_W, CHAR_I, CHAR_N, CHAR_S, // UNDER (index 21) is replaced with number of P2 wins
	 SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE};

char MessageP1Start[] = // scrolls "Player 1 - Place Ships" on Marquee display
	{SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,
	 CHAR_P, CHAR_L, CHAR_A, CHAR_Y, CHAR_E, CHAR_R, SPACE,
	 CHAR_1, SPACE, DASH, SPACE,
	 CHAR_P, CHAR_L, CHAR_A, CHAR_C, CHAR_E, SPACE,
	 CHAR_S, CHAR_H, CHAR_I, CHAR_P, CHAR_S,
	 SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE};
char MessageP2Start[] = // scrolls "Player 2 - Place Ships" on Marquee display
	{SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,
	 CHAR_P, CHAR_L, CHAR_A, CHAR_Y, CHAR_E, CHAR_R, SPACE,
	 CHAR_2, SPACE, DASH, SPACE,
	 CHAR_P, CHAR_L, CHAR_A, CHAR_C, CHAR_E, SPACE,
	 CHAR_S, CHAR_H, CHAR_I, CHAR_P, CHAR_S,
	 SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE};

/**
 * Each round starts with player 1's move - so the round number is displayed at player 1's turn.
 * If the number of rounds is greater than 0xF, an additional digit is included
 */
char MessageP1Turn[] = // scrolls "Round _ - Player 1" on Marquee display if numRounds <= 0xF
	{SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,
	 CHAR_R, CHAR_O, CHAR_U, CHAR_N, CHAR_D, SPACE, UNDER, SPACE, DASH, SPACE, // UNDER (index 16) is replaced with round number
	 CHAR_P, CHAR_L, CHAR_A, CHAR_Y, CHAR_E, CHAR_R, SPACE, CHAR_1,
	 SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE};
char MessageP1TurnEXT[] = // scrolls "Round __ - Player 1" on Marquee display if numRounds > 0xF
	{SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,
	 CHAR_R, CHAR_O, CHAR_U, CHAR_N, CHAR_D, SPACE, UNDER, UNDER, SPACE, DASH, SPACE, // UNDER (index 16 & 17) is replaced with round numbers
	 CHAR_P, CHAR_L, CHAR_A, CHAR_Y, CHAR_E, CHAR_R, SPACE, CHAR_1,
	 SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE};
char MessageP2Turn[] = // scrolls "Round _ - Player 2" on Marquee display if numRounds <= 0xF
	{SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,
	 CHAR_P, CHAR_L, CHAR_A, CHAR_Y, CHAR_E, CHAR_R, SPACE, CHAR_2,
	 SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE};

char MessageHit[] = // scrolls "HIT" on Marquee display
	{SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,
	 CHAR_H, CHAR_I, CHAR_T,
	 SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE};
char MessageMiss[] = // scrolls "MISS" on Marquee display
	{SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,
	 CHAR_M, CHAR_I, CHAR_S, CHAR_S,
	 SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE};

char MessageP1Wins[] = // scrolls "Player 1 Wins" on Marquee display
	{SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,
	 CHAR_P, CHAR_L, CHAR_A, CHAR_Y, CHAR_E, CHAR_R, SPACE,
	 CHAR_1, SPACE,
	 CHAR_W, CHAR_I, CHAR_N, CHAR_S,
	 SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE};
char MessageP2Wins[] = // scrolls "Player 2 Wins" on Marquee display
	{SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,
	 CHAR_P, CHAR_L, CHAR_A, CHAR_Y, CHAR_E, CHAR_R, SPACE,
	 CHAR_2, SPACE,
	 CHAR_W, CHAR_I, CHAR_N, CHAR_S,
	 SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE};

/*** Enumerated variables ***/
typedef enum gameState {title = 0, playerOneStart = 1, playerTwoStart = 2,
						playerOneTurn = 3, playerTwoTurn = 4, endGame = 5} gameState; // represents all states the game can be in
						gameState currGameState = title; // start the game at the title sequence

typedef enum LEDstate {off = 0, targetMiss = 1, targetHit = 2, blink = 3} LEDstate;
					   LEDstate currCursor = blink; // the cursor will always blink

typedef enum cursorOrient {horizontalCursor = 0, verticalCursor = 1} cursorOrient;
				   	       cursorOrient currCursorOrient = horizontalCursor; // start the cursor in a horizontal orientation

typedef enum shipType {singleShip = 0, doubleShip = 1} shipType;
					   shipType currShipType = singleShip; // start with a single ship

typedef enum moveState {idleState = 0, fireState = 1} moveState;
					 	moveState currMoveState = idleState; // store the state of the current move

typedef enum moveResult {missResult = 0, hitResult = 1} moveResult;
						 moveResult currMoveResult; // store the result of the most recent move

/*** Data structures ***/
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
	char numWins;
} player;

/*** Instances of maps ***/
// Player One
bitMap playerOneShips = {
	.horizontal_bitMap = { 0 },
	.vertical_bitMap = { 0 }

//	.horizontal_bitMap = {
//		{ off, off, off, targetMiss, targetMiss, off, off, off },
//		{ off, off, off, targetHit, targetHit, off, off, off },
//		{ off, off, off, blink, blink, off, off, off },
//	},
//
//	.vertical_bitMap = {
// 		{ off, targetHit, targetHit, off, off, targetMiss, targetMiss, off,
// 		  targetHit, targetMiss, targetHit, blink, targetHit, targetHit, targetMiss, targetMiss },
//
//	   { off, targetHit, targetHit, off, off, targetMiss, targetMiss, off,
//	     targetHit, off, off, blink, targetHit, targetHit, targetMiss, targetMiss },
//	}
};
// Player Two
bitMap playerTwoShips = {
	.horizontal_bitMap = { 0 },
	.vertical_bitMap = { 0 }
};

/*** Instance of players ***/
// Player One
player playerOne = {
		.ownMap = &playerOneShips,
		.opponentMap = &playerTwoShips,
		.singleBoatsRemaining = 3,
		.doubleBoatsRemaining = 2,
		.hits = 0,
		.numWins = 0
};
// Player Two
player playerTwo = {
		.ownMap = &playerTwoShips,
		.opponentMap = &playerOneShips,
		.singleBoatsRemaining = 3,
		.doubleBoatsRemaining = 2,
		.hits = 0,
		.numWins = 0
};

	///////////////////////////////////
	////// Function Declarations //////
	///////////////////////////////////

bitMap buildCursorBoard(int hPot, int vPot, enum cursorOrient orient);
bitMap compileBoard(bitMap *cursor, bitMap *map);
void assignIndex_State(char row, char col, bitMap *map, cursorOrient orient, LEDstate state);
void drawBoard(bitMap *map);

bitMap placeShips(); // TODO
void fireShot(player *player); // TODO in progress

	//////////////////////////////////
	////// Function Definitions //////
	//////////////////////////////////

/**
 * Process inputs from push buttons and potentiometers
 */
int input(void) {
	actionButton_in = (GPIOC->IDR >> 10) & 1; // parse only the switch connected to PC10
	toggleButton_in = (GPIOC->IDR >> 11) & 1; // parse only the switch connected to PC11
	shipTypeSwitch_in = GPIOC->IDR & 1; // parse only PC0

	/* lock in action trigger */
	if (actionButton_in == 0) {
		currMoveState = fireState;
		HAL_Delay(pushButton_delay);
	} else {
		currMoveState = idleState;
		HAL_Delay(pushButton_delay);
	}

	/* toggle orientation of cursor */
	if (toggleButton_in == 0) {
		if (currCursorOrient == verticalCursor) { // the cursor is horizontal
			currCursorOrient = horizontalCursor;
			HAL_Delay(pushButton_delay);
		} else {
			currCursorOrient = verticalCursor;
			HAL_Delay(pushButton_delay);
		}
	}

	/* toggle type of ship */
	if (shipTypeSwitch_in == 1) { // slider switch at PC0 is high
		currShipType = doubleShip;
	} else {
		currShipType = singleShip;
	}

	/* read potentiometers */
		// Horizontal
		ADC1->SQR3 = 1; // select channel 1
		ADC1->CR2 |= 1<<30; // start sequence of reading & converting channel 1
		while(!(ADC1->SR & 1<<1));
		potHorizontal_in = ADC1->DR; // read value from potentiometer at PA1

		// Vertical
		ADC1->SQR3 = 2; // select channel 2
		ADC1->CR2 |= 1<<30; // start sequence of reading & converting channel 2
		while(!(ADC1->SR & 1<<1));
		potVertical_in = ADC1->DR; // read value from potentiometer at PA2

	return 0;
}

/**
 * Game logic that reads inputs and produces outputs
 */
int logic (void) {

	switch (currGameState) {

		case playerOneStart:
			if (playerOneShipsPlaced) {
				currGameState = playerTwoStart;
			}
			break;

		case playerTwoStart:
			if (playerTwoShipsPlaced) {
				currGameState = playerOneTurn;
			}
			break;

		case playerOneTurn:
			numRounds++; // rounds start with player one's move
			if (currMoveState == fireState) {
				currMoveState = idleState;
				fireShot(&playerOne);
				if (playerOne.hits == 7) {
					currGameState = endGame; // game ends
					break;
				}
				currGameState = playerTwoTurn; // toggle  to player two
			}
			break;

		case playerTwoTurn:
			if (currMoveState == fireState) {
				currMoveState = idleState;
				fireShot(&playerTwo);
				if (playerTwo.hits == 7) {
					currGameState = endGame; // game ends
					break;
				}
				currGameState = playerOneTurn; // toggle  to player one
			}
			break;

		case endGame:
			if (playerOne.hits == 7) {
				playerOne.numWins++;
			} else {
				playerTwo.numWins++;
			}

			if (playAgain == 1) {
//TODO logic
				firstGame = false;
			}
			break;
	}

	return 0;
}

/**
 * prepares output and draws board to 7SEG display
 */
int output (void) {

	/* (TODO delete) TESTING */
	bitMap cursorBoard = buildCursorBoard(potHorizontal_in, potVertical_in, currCursorOrient);
	bitMap display = compileBoard(&cursorBoard, &playerOneShips);
	drawBoard(&display);

	displayToggle = 0; // set to 1 to toggle Marquee display
	switch (currGameState) {

		case title:
			/* Display Game Title */
			if (displayToggle & 1) {
				displayToggle = 0;
				Animate_On = 1; // turn on animation so that interrupt displays Message[]
				Message_Pointer = &MessageTitle[0];
				Save_Pointer = &MessageTitle[0];
				Message_Length = sizeof(MessageTitle)/sizeof(MessageTitle[0]);
				Delay_msec = 200;
				HAL_Delay(title_delay);           // Delay 5 seconds to allow message to scroll
				Animate_On = 0;            // Stop scrolling message
				HAL_Delay(displayHold_delay);           // Delay to give space
			}
			if (!firstGame) {
				MessageP1Recap[21] = playerOne.numWins;
				Animate_On = 1; // turn on animation so that interrupt displays Message[]
				Message_Pointer = &MessageP1Recap[0];
				Save_Pointer = &MessageP1Recap[0];
				Message_Length = sizeof(MessageP1Recap)/sizeof(MessageP1Recap[0]);
				Delay_msec = 200;
				HAL_Delay(recap_delay);           // Delay 5 seconds to allow message to scroll
				Animate_On = 0;            // Stop scrolling message
				HAL_Delay(displayHold_delay);           // Delay to give space

				MessageP2Recap[21] = playerTwo.numWins;
				Animate_On = 1; // turn on animation so that interrupt displays Message[]
				Message_Pointer = &MessageP2Recap[0];
				Save_Pointer = &MessageP2Recap[0];
				Message_Length = sizeof(MessageP2Recap)/sizeof(MessageP2Recap[0]);
				Delay_msec = 200;
				HAL_Delay(recap_delay);           // Delay 5 seconds to allow message to scroll
				Animate_On = 0;            // Stop scrolling message
				HAL_Delay(displayHold_delay);           // Delay to give space
			}

			currGameState = playerOneStart; // start game
			break;

		case playerOneStart:
			/* Display Player One Start Screen*/
			if (displayToggle & 1) {
				displayToggle = 0;
				Animate_On = 1; // turn on animation so that interrupt displays Message[]
				Message_Pointer = &MessageP1Start[0];
				Save_Pointer = &MessageP1Start[0];
				Message_Length = sizeof(MessageP1Start)/sizeof(MessageP1Start[0]);
				Delay_msec = 200;
				HAL_Delay(playerStart_delay);          // Delay to allow message to scroll
				Animate_On = 0;            // Stop scrolling message
				HAL_Delay(displayHold_delay);           // Delay to give space
			}

			break;

		case playerTwoStart:
			/* Display Player Two Start Screen*/
			if (displayToggle & 1) {
				displayToggle = 0;
				Animate_On = 1; // turn on animation so that interrupt displays Message[]
				Message_Pointer = &MessageP2Start[0];
				Save_Pointer = &MessageP2Start[0];
				Message_Length = sizeof(MessageP2Start)/sizeof(MessageP2Start[0]);
				Delay_msec = 200;
				HAL_Delay(playerStart_delay);          // Delay to allow message to scroll
				Animate_On = 0;            // Stop scrolling message
				HAL_Delay(displayHold_delay);           // Delay to give space
			}

			break;

		case playerOneTurn:
			/**
			 * Display player one's turn
			 */
			if (displayToggle & 1) {
				displayToggle = 0;
				displayRound_0th = numRounds & 0xF; // there will always be a 0th position round value to display
				/**
				 * The number of rounds is represented with a hexadecimal value. If the number of rounds played
				 * exceeds 15 (0xF), another digit must be included in the message.
				 */
				if(numRounds <= 0xF) {
					MessageP1Turn[16] = displayRound_0th; // update round number (0th position)
					Animate_On = 1; // turn on animation so that interrupt displays Message[]
					Message_Pointer = &MessageP1Turn[0];
					Save_Pointer = &MessageP1Turn[0];
					Message_Length = sizeof(MessageP1Turn)/sizeof(MessageP1Turn[0]);
					Delay_msec = 100;
					HAL_Delay(round_delay);           // Delay to allow message to scroll
					Animate_On = 0;            // Stop scrolling message
					HAL_Delay(displayHold_delay);           // Delay to give space
				} else {
					MessageP1Turn[17] = displayRound_0th; // update round number (0th position)
					MessageP1Turn[16] = displayRound_1st; // update round number (1st position)
					displayRound_1st = numRounds>>0xF; // include bit 1 of numRounds if numRounds > 15
					Animate_On = 1; // turn on animation so that interrupt displays Message[]
					Message_Pointer = &MessageP1TurnEXT[0];
					Save_Pointer = &MessageP1TurnEXT[0];
					Message_Length = sizeof(MessageP1TurnEXT)/sizeof(MessageP1TurnEXT[0]);
					Delay_msec = 100;
					HAL_Delay(extRound_delay);          // Delay to allow message to scroll
					Animate_On = 0;            // Stop scrolling message
					HAL_Delay(displayHold_delay);           // Delay to give space
				}

				currMoveResult = hitResult; // TODO delete
				switch(currMoveResult) {
					case hitResult:
						Animate_On = 1; // turn on animation so that interrupt displays Message[]
						Message_Pointer = &MessageHit[0];
						Save_Pointer = &MessageHit[0];
						Message_Length = sizeof(MessageHit)/sizeof(MessageHit[0]);
						Delay_msec = 100;
						HAL_Delay(move_delay);          // Delay to allow message to scroll
						Animate_On = 0;            // Stop scrolling message
						HAL_Delay(displayHold_delay);           // Delay to give space
						break;
					case missResult:
						Animate_On = 1; // turn on animation so that interrupt displays Message[]
						Message_Pointer = &MessageMiss[0];
						Save_Pointer = &MessageMiss[0];
						Message_Length = sizeof(MessageMiss)/sizeof(MessageMiss[0]);
						Delay_msec = 100;
						HAL_Delay(move_delay);          // Delay to allow message to scroll
						Animate_On = 0;            // Stop scrolling message
						HAL_Delay(displayHold_delay);           // Delay to give space
						break;
				}
			}

			break;

		case playerTwoTurn:
			/**
			 * Display player two's turn
			 */
			if (displayToggle & 1) {
				displayToggle = 0;
				displayRound_0th = numRounds & 0xF; // there will always be a 0th position round value to display
				/**
				 * Second half of each round (don't need to update round number)
				 */
					Animate_On = 1; // turn on animation so that interrupt displays Message[]
					Message_Pointer = &MessageP2Turn[0];
					Save_Pointer = &MessageP2Turn[0];
					Message_Length = sizeof(MessageP2Turn)/sizeof(MessageP2Turn[0]);
					Delay_msec = 100;
					HAL_Delay(round_delay);           // Delay to allow message to scroll
					Animate_On = 0;            // Stop scrolling message
					HAL_Delay(displayHold_delay);           // Delay to give space

				switch(currMoveResult) {
					case hitResult:
						Animate_On = 1; // turn on animation so that interrupt displays Message[]
						Message_Pointer = &MessageHit[0];
						Save_Pointer = &MessageHit[0];
						Message_Length = sizeof(MessageHit)/sizeof(MessageHit[0]);
						Delay_msec = 100;
						HAL_Delay(move_delay);          // Delay to allow message to scroll
						Animate_On = 0;            // Stop scrolling message
						HAL_Delay(displayHold_delay);           // Delay to give space
						break;
					case missResult:
						Message_Pointer = &MessageMiss[0];
						Save_Pointer = &MessageMiss[0];
						Message_Length = sizeof(MessageMiss)/sizeof(MessageMiss[0]);
						Delay_msec = 100;
						HAL_Delay(move_delay);          // Delay to allow message to scroll
						Animate_On = 0;            // Stop scrolling message
						HAL_Delay(displayHold_delay);           // Delay to give space
						break;
				}
			}

			break;

		/**
		 * Prompt players to restart the game and preserve win record with points? If we have the time
		 */
		case endGame:
			/* Display Game End Screen*/
			if (playerOne.hits == 7) { // player one won
				Animate_On = 1; // turn on animation so that interrupt displays Message[]
				Message_Pointer = &MessageP1Wins[0];
				Save_Pointer = &MessageP1Wins[0];
				Message_Length = sizeof(MessageP1Wins)/sizeof(MessageP1Wins[0]);
				Delay_msec = 200;
				HAL_Delay(winner_delay);          // Delay to allow message to scroll
				Animate_On = 0;            // Stop scrolling message
				HAL_Delay(displayHold_delay);           // Delay to give space
			} else { // player two won
				Animate_On = 1; // turn on animation so that interrupt displays Message[]
				Message_Pointer = &MessageP2Wins[0];
				Save_Pointer = &MessageP2Wins[0];
				Message_Length = sizeof(MessageP2Wins)/sizeof(MessageP2Wins[0]);
				Delay_msec = 200;
				HAL_Delay(winner_delay);          // Delay to allow message to scroll
				Animate_On = 0;            // Stop scrolling message
				HAL_Delay(displayHold_delay);           // Delay to give space
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
bitMap buildCursorBoard(int hPot, int vPot, enum cursorOrient orient) {
	bitMap cursorBoard = {
			.horizontal_bitMap = { 0 },
			.vertical_bitMap = { 0 }
	};
	char row, col;

	/* determine column & row */
	if (orient == horizontalCursor) { // horizontal segments
		if (currShipType == singleShip) {
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
		} else { // handle double ship logic for horizontal orientation ***************TODO
			/**
			 * For double ships, the cursor will be set to the position of col & col+1
			 */
			//horizontal movement
				if (hPot < 585) { // leftmost position = leftmost segment
					col = 0; // & 1
				} else if (hPot < 1170) {
					col = 1; // & 2
				} else if (hPot < 1755) {
					col = 2; // & 3
				} else if (hPot < 2341) { // middle position = middle segment
					col = 3; // & 4
				} else if (hPot < 2926) {
					col = 4; // & 5
				} else if (hPot < 3511) {
					col = 5; // & 6
				} else { // rightmost position = rightmost segment
					col = 6; // & 7
				}
			//vertical movement functionality is unchanged
				if (vPot < 1365) { // left position = top
					row = 0;
				} else if (vPot < 2730) { // middle position = middle
					row = 1;
				} else { // right position = bottom
					row = 2;
				}

			assignIndex_State(row, col, &cursorBoard, horizontalCursor, currCursor);
			assignIndex_State(row, col+1, &cursorBoard, horizontalCursor, currCursor);
		}


	} else { // vertical segments
		if (currShipType == singleShip) {
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
		} else { // handle double ship logic for vertical orientation ***************TODO
			/**
			 * For double ships, the cursor will be set to the position of row & row+1
			 */
		//horizontal movement functionality is unchanged
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
			row = 0; // & 1 - the double ship must take up both vertical segments

			assignIndex_State(row, col, &cursorBoard, verticalCursor, currCursor);
			assignIndex_State(row+1, col, &cursorBoard, verticalCursor, currCursor);
		}
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
			if (cursor->horizontal_bitMap[row][col] == blink) { // if the cursor is on the position
				comp.horizontal_bitMap[row][col] = currCursor;
			} else {
				comp.horizontal_bitMap[row][col] = map->horizontal_bitMap[row][col]; // set comp board position to that of map
			}
		}
	}
	/* compile vertical */
	for (int row = 0; row < 2; row++) {
		for (int col = 0; col < 16; col++) {
			if (cursor->vertical_bitMap[row][col] == blink) { // if the cursor is on the position
				comp.vertical_bitMap[row][col] = currCursor;
			} else {
				comp.vertical_bitMap[row][col] = map->vertical_bitMap[row][col]; // set comp board position to that of map
			}
		}
	}

	return comp;
}
void drawBoard(bitMap *map) {

	char seg[8] = { 0 };
	bool dimOn = (timer % 8 == 0); // sets PWM for dimming
	bool blinkOn = ((timer/250) % 2 == 0); // sets PWM for blinking

	/* set the horizontal segments */
	for (int i = 0; i < 8; i++)	{

		// set the top segments
		switch(map->horizontal_bitMap[0][i]) {
			case off:
				break;
			case targetMiss:
				seg[i] |= ( dimOn ) ? 1 : 0; // if PWM for dimming is high, set to 1, otherwise 0
				break;
			case targetHit:
				seg[i] |= 1;
				break;
			case blink:
				seg[i] |= ( blinkOn ) ? 1 : 0;
				break;
		}

		// set the middle segments
		switch(map->horizontal_bitMap[1][i]) {
			case off:
				break;
			case targetMiss:
				seg[i] |= ( dimOn ) ? 1<<6 : 0;
				break;
			case targetHit:
				seg[i] |= 1<<6;
				break;
			case blink:
				seg[i] |= ( blinkOn ) ? 1<<6 : 0;
				break;
		}

		// set the bottom segments
		switch(map->horizontal_bitMap[2][i]) {
			case off:
				break;
			case targetMiss:
				seg[i] |= ( dimOn ) ? 1<<3 : 0;
				break;
			case targetHit:
				seg[i] |= 1<<3;
				break;
			case blink:
				seg[i] |= ( blinkOn ) ? 1<<3 : 0;
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
			case targetMiss:
				seg[i/2] |= ( dimOn ) ? 1<<5 : 0;
				break;
			case targetHit:
				seg[i/2] |= 1<<5;
				break;
			case blink:
				seg[i/2] |= ( blinkOn ) ? 1<<5 : 0;
				break;
		}

		// set the bottom left segments
		switch(map->vertical_bitMap[1][i]) {
			case off:
				break;
			case targetMiss:
				seg[i/2] |= ( dimOn ) ? 1<<4 : 0;
				break;
			case targetHit:
				seg[i/2] |= 1<<4;
				break;
			case blink:
				seg[i/2] |= ( blinkOn ) ? 1<<4 : 0;
				break;
		}

		/* right */
		i++; // move to next index of vertical array
		// set the top right segments
		switch(map->vertical_bitMap[0][i]) {
			case off:
				break;
			case targetMiss:
				seg[i/2] |= ( dimOn ) ? 1<<1 : 0;
				break;
			case targetHit:
				seg[i/2] |= 1<<1;
				break;
			case blink:
				seg[i/2] |= ( blinkOn ) ? 1<<1 : 0;
				break;
		}

		// set the bottom right segments
		switch(map->vertical_bitMap[1][i]) {
			case off:
				break;
			case targetMiss:
				seg[i/2] |= ( dimOn ) ? 1<<2 : 0;
				break;
			case targetHit:
				seg[i/2] |= 1<<2;
				break;
			case blink:
				seg[i/2] |= ( blinkOn ) ? 1<<2 : 0;
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

void fireShot(player *player) {

	return;
}

