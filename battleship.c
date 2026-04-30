#include "battleship.h"

/*******************************/
/******   WORKING NOTES   ******/
/*******************************/

	//////////////////////////////////
	////////////// Data //////////////
	//////////////////////////////////

static int timer = 0; // Used to control PWM
static int currCursorPosition[2]; // Track the specific location of the cursor
							  // Index 0: row
							  // Index 1: col

/*** Delay Variables to be used in HAL_Delay ***/
extern int pushButton_delay = 300;
extern int displayHold_delay = 1000;
extern int scrollSpeed_norm = 150;
extern int scrollSpeed_fast = 100;
extern int titleScroll_delay = 3300;
extern int recapScroll_delay = 8000;
extern int playerStartScroll_delay = 10000;
extern int roundScroll_delay = 2000;
extern int extRoundScroll_delay = 2100;
extern int moveScroll_delay = 1800;
extern int winnerScroll_delay = 5000;
extern int endGameScroll_delay = 10000;

/*** Simple Data Types ***/
// Input Variables
int actionButton_in = 0; // Push button 10 (PC10) used to execute action (place boat & fire)
int toggleButton_in = 0; // Push button 11 (PC11) used to toggle cursor between horizontal (0) and vertical (1)
int shipTypeSwitch_in = 0; // switch 8 (PC0) used to switch from 1 space (0) to 2 space (1) ships when placing
int potHorizontal_in = 0; // Potentiometer (PA1) used to control horizontal cursor movement
int potVertical_in = 0; // Potentiometer (PA2) used to control vertical cursor movement

// Logic Variables
bool playerOneShipsPlaced = false; // Track when player one finishes placing ships
bool playerTwoShipsPlaced = false; // Track when player two finishes placing ships
char numRounds = 0; // Track number of rounds played
bool playAgain = false; // Detect if player wants to play again
bool firstGame = true; // Detect if this is the first game played

// Output Variables
char displayRound_0th = 0x0; // The 0th position of the number of rounds played
char displayRound_1st = 0x0; // The 1st position of the number of rounds played

/*** Marquee Messages ***/
char MessageTitle[] = // Scrolls "Battleship" on Marquee display
	{SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,
	 CHAR_B, CHAR_A, CHAR_T, CHAR_T, CHAR_L, CHAR_E,
	 CHAR_S, CHAR_H, CHAR_I, CHAR_P,
	 SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE};

char MessageP1Recap[] = // Scrolls "Player 1 - _ Wins" on Marquee display
	{SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,
	 CHAR_P, CHAR_L, CHAR_A, CHAR_Y, CHAR_E, CHAR_R, SPACE,
	 CHAR_1, SPACE, DASH, SPACE,
	 UNDER, SPACE, CHAR_W, CHAR_I, CHAR_N, CHAR_S, // UNDER (index 21) is replaced with number of P1 wins
	 SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE};
char MessageP2Recap[] = // Scrolls "Player 2 - _ Wins" on Marquee display
	{SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,
	 CHAR_P, CHAR_L, CHAR_A, CHAR_Y, CHAR_E, CHAR_R, SPACE,
	 CHAR_2, SPACE, DASH, SPACE,
	 UNDER, SPACE, CHAR_W, CHAR_I, CHAR_N, CHAR_S, // UNDER (index 21) is replaced with number of P2 wins
	 SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE};

char MessageP1Start[] = // Scrolls "Player 1 - Place Ships" on Marquee display
	{SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,
	 CHAR_P, CHAR_L, CHAR_A, CHAR_Y, CHAR_E, CHAR_R, SPACE,
	 CHAR_1, SPACE, DASH, SPACE,
	 CHAR_P, CHAR_L, CHAR_A, CHAR_C, CHAR_E, SPACE,
	 CHAR_S, CHAR_H, CHAR_I, CHAR_P, CHAR_S,
	 SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE};
char MessageP2Start[] = // Scrolls "Player 2 - Place Ships" on Marquee display
	{SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,
	 CHAR_P, CHAR_L, CHAR_A, CHAR_Y, CHAR_E, CHAR_R, SPACE,
	 CHAR_2, SPACE, DASH, SPACE,
	 CHAR_P, CHAR_L, CHAR_A, CHAR_C, CHAR_E, SPACE,
	 CHAR_S, CHAR_H, CHAR_I, CHAR_P, CHAR_S,
	 SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE};

/**
 * If the number of rounds is greater than 0xF, an additional digit is included in MessageRoundStartEXT[]
 */
char MessageRoundStart[] = // Scrolls "Round _" on Marquee display in numRounds <= 0xF
	{SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,
	 CHAR_R, CHAR_O, CHAR_U, CHAR_N, CHAR_D, SPACE, UNDER, SPACE, DASH, SPACE, // UNDER (index 16) is replaced with round number
	 SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE};
char MessageRoundStartEXT[] = // Scrolls "Round __" on Marquee display if numRounds > 0xF
	{SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,
	 CHAR_R, CHAR_O, CHAR_U, CHAR_N, CHAR_D, SPACE, UNDER, UNDER, SPACE, DASH, SPACE, // UNDER (index 16 & 17) is replaced with round numbers
	 CHAR_P, CHAR_L, CHAR_A, CHAR_Y, CHAR_E, CHAR_R, SPACE, CHAR_1,
	 SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE};

char MessageP1Turn[] = // Scrolls "Player 1" on Marquee display
	{SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,
	 CHAR_P, CHAR_L, CHAR_A, CHAR_Y, CHAR_E, CHAR_R, SPACE, CHAR_1,
	 SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE};
char MessageP2Turn[] = // Scrolls "Player 2" on Marquee display
	{SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,
	 CHAR_P, CHAR_L, CHAR_A, CHAR_Y, CHAR_E, CHAR_R, SPACE, CHAR_2,
	 SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE};

char MessageHit[] = // Scrolls "HIT" on Marquee display
	{SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,
	 CHAR_H, CHAR_I, CHAR_T,
	 SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE};
char MessageMiss[] = // Scrolls "MISS" on Marquee display
	{SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,
	 CHAR_M, CHAR_I, CHAR_S, CHAR_S,
	 SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE};

char MessageP1Wins[] = // Scrolls "Player 1 Wins" on Marquee display
	{SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,
	 CHAR_P, CHAR_L, CHAR_A, CHAR_Y, CHAR_E, CHAR_R, SPACE,
	 CHAR_1, SPACE,
	 CHAR_W, CHAR_I, CHAR_N, CHAR_S,
	 SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE};
char MessageP2Wins[] = // Scrolls "Player 2 Wins" on Marquee display
	{SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,
	 CHAR_P, CHAR_L, CHAR_A, CHAR_Y, CHAR_E, CHAR_R, SPACE,
	 CHAR_2, SPACE,
	 CHAR_W, CHAR_I, CHAR_N, CHAR_S,
	 SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE};

/*** Enumerated variables ***/
typedef enum gameState {title = 0, roundStart = 1, playerOneStart = 2, playerTwoStart = 3,
						playerOneTurn = 4, playerTwoTurn = 5, endGame = 6} gameState; // represents all states the game can be in
						gameState currGameState = title; // start the game at the title sequence

typedef enum LEDstate {off = 0, targetMiss = 1, targetHit = 2, blink = 3} LEDstate;
					   LEDstate cursorState = blink; // She cursor will always blink

typedef enum positionState {isEmpty = false, hasShip = true} positionState;

typedef enum cursorOrient {horizontalCursor = 0, verticalCursor = 1} cursorOrient;
				   	       cursorOrient currCursorOrient = horizontalCursor; // Start the cursor in a horizontal orientation

typedef enum shipType {singleShip = 0, doubleShip = 1} shipType;
					   shipType currShipType = singleShip; // Start with a single ship

typedef enum moveState {idleState = 0, actionState = 1} moveState;
					 	moveState currMoveState = idleState; // Store the state of the current move

typedef enum moveResult {noResult = 0, missResult = 1, hitResult = 2} moveResult;
						 moveResult currMoveResult = noResult; // Start with no moves made

typedef enum displayState {scroll = 0, play = 1} displayState;
						   displayState currDisplayState; // Toggle message (scroll) or board (play) display

/*** Data structures ***/
/**
 * Bitmaps are composed of horizontal and vertical components and track hits and misses on the board
 * as well as the cursor position
 */
typedef struct {
	LEDstate horizontal_LEDMap[3][8]; // This array displays the player's horizontal hits and misses
	LEDstate vertical_LEDMap[2][16]; // This array displays the player's vertical hits and misses
	positionState horizontal_shipMap[3][8]; // This array holds the position of a player's horizontal ships
	positionState vertical_shipMap[2][16]; // This array holds the position of a player's vertical ships
} bitMap;

/**
 * Players have their own board that they place boats on & an opponent board that they try to sink the ships on
 */
typedef struct {
	bitMap *ownMap; // Points to board with their own ships
	bitMap *opponentMap; // Points to board with their opponent's ships
	char doubleBoatProperties[2][3]; // Track details about the two double boats
									 // The second half of the ship is at row+1 if vertical, col+1 if horizontal
									 /* Boat 1 [0][i] */
										 //	i=0: horizontal (0) or vertical (1)
										 //	i=1: row
										 //	i=2: col
									 /* Boat 2 [1][i] */
										 //	i=0: horizontal (0) or vertical (1)
										 //	i=1: row
										 //	i=2: col
	char singleBoatsRemaining; // Track number of remaining single-spaced boats
	char doubleBoatsRemaining; // Track number of remaining double-spaced boats
	char numHits; // Track total number of hits
	char numWins; // Track total number of wins
} player;

/*** Instances of maps ***/
static bitMap cursorBoard = { // store game board with the position of the cursor
		.horizontal_LEDMap = { 0 },
		.vertical_LEDMap = { 0 }
};

// Player One
bitMap playerOneShips = {
	.horizontal_LEDMap = { 0 },
	.vertical_LEDMap = { 0 },
	.horizontal_shipMap = { 0 },
	.vertical_shipMap = { 0 },
//
//	.horizontal_LEDMap = {
//		{ targetMiss, off, off, targetMiss, targetMiss, off, off, off },
//		{ targetMiss, off, off, targetHit, targetHit, off, off, off },
//		{ targetMiss, off, off, blink, blink, off, off, off },
//	},
//
//	.vertical_LEDMap = {
// 		{ off, targetHit, targetHit, off, off, targetMiss, targetMiss, off,
// 		  targetHit, targetMiss, targetHit, blink, targetHit, targetHit, targetMiss, targetMiss },
//
//	   { off, targetHit, targetHit, off, off, targetMiss, targetMiss, off,
//	     targetHit, off, off, blink, targetHit, targetHit, targetMiss, targetMiss },
//	}
};
// Player Two
bitMap playerTwoShips = {
	.horizontal_LEDMap = { 0 },
	.vertical_LEDMap = { 0 },
	.horizontal_shipMap = { 0 },
	.vertical_shipMap = { 0 }
};

/*** Instance of players ***/
// Player One
player playerOne = {
	.ownMap = &playerOneShips,
	.opponentMap = &playerTwoShips,
	.singleBoatsRemaining = 0, // Increment as ships are placed, decrement as ships are sunk
	.doubleBoatsRemaining = 0, // Increment as ships are placed, decrement as ships are sunk
	.numHits = 0,
	.numWins = 0
};
// Player Two
player playerTwo = {
	.ownMap = &playerTwoShips,
	.opponentMap = &playerOneShips,
	.singleBoatsRemaining = 0, // Increment as ships are placed, decrement as ships are sunk
	.doubleBoatsRemaining = 0, // Increment as ships are placed, decrement as ships are sunk
	.numHits = 0,
	.numWins = 0
};

	///////////////////////////////////
	////// Function Declarations //////
	///////////////////////////////////

// Process inputs
void input (void);
// Interpret inputs & coordinate outputs
void logic (void);
// Determine message to be written to 7SEG display
void outputMessage (void);
// Set message to be displayed
void displayMessage(char *messageNamePointer, int messageLength, int scrollSpeed, int messageDelay, int holdDelay);
// Determine & set position of cursor on board
bitMap* buildCursorBoard(int hPot, int vPot, enum cursorOrient orient);
// Set cursor on game board
bitMap* compileBoard(bitMap *cursorPointer, bitMap *boardPointer);
// Set a specific position on a map to a given LEDstate
void assignIndex_LEDState(bitMap *currBoardPointer, char row, char col, cursorOrient orient, LEDstate state);
// Prepare game board to be written to 7SEG display
void drawBoard(bitMap *currDisplayBoardPointer);
// Set the positions of player's ships
void placeShip(player *playerPointer);
// Make move
void fireShot(player *playerPointer);
// Prepare the game to be played again
void resetGame(void);

	//////////////////////////////////
	////// Function Definitions //////
	//////////////////////////////////

/**
 * Provide timer variable for PWM and refresh 7SEG display map
 */
void handle_interrupts (void) {

	timer++;
	if (currDisplayState == play) {
		drawBoard(&currDisplayBoard); // Refresh game board
	}

	return;
}

/**
 * Process inputs from push buttons and potentiometers
 */
void input(void) {
	actionButton_in = (GPIOC->IDR >> 10) & 1; // Parse only the switch connected to PC10
	toggleButton_in = (GPIOC->IDR >> 11) & 1; // Parse only the switch connected to PC11
	shipTypeSwitch_in = GPIOC->IDR & 1; // Parse only PC0

	/* Lock in action trigger */
	if (actionButton_in == 0) {
		currMoveState = actionState;
		HAL_Delay(pushButton_delay);
	} else {
		currMoveState = idleState;
		HAL_Delay(pushButton_delay);
	}

	/* Toggle orientation of cursor */
	if (toggleButton_in == 0) {
		if (currCursorOrient == verticalCursor) { // The cursor is horizontal
			currCursorOrient = horizontalCursor;
			HAL_Delay(pushButton_delay);
		} else {
			currCursorOrient = verticalCursor;
			HAL_Delay(pushButton_delay);
		}
	}

	/* Toggle type of ship */
	if (shipTypeSwitch_in == 1) { // Slider switch at PC0 is high
		currShipType = doubleShip;
	} else {
		currShipType = singleShip;
	}

	/* Read potentiometers */
		// Horizontal
		ADC1->SQR3 = 1; // Select channel 1
		ADC1->CR2 |= 1<<30; // Start sequence of reading & converting channel 1
		while(!(ADC1->SR & 1<<1));
		potHorizontal_in = ADC1->DR; // Read value from potentiometer at PA1

		// Vertical
		ADC1->SQR3 = 2; // Select channel 2
		ADC1->CR2 |= 1<<30; // Start sequence of reading & converting channel 2
		while(!(ADC1->SR & 1<<1));
		potVertical_in = ADC1->DR; // Read value from potentiometer at PA2

	return;
}

/**
 * Game logic that interprets inputs and coordinates outputs
 */
void logic (void) {

	switch (currGameState) {

	/**
	 * Enter from program start OR "endGame" (if player wants to playAgain)
	 * display message
	 * Move to "playerOneStart"
	 */
		case title:
			currDisplayState = scroll; // this state only displays a message
			outputMessage(); // Read the game start sequence
			currGameState = playerOneStart; // Move to next state
			// Do not break, pass directly into playerOneStart while currDisplayState is scroll
	/**
	 * Enter from "title"
	 * Display message
	 * Player one places ships
	 * Move to "playerTwoStart"
	 */
		case playerOneStart:
			// enter on first pass from case "title"
			if (currDisplayState == scroll) {
				outputMessage(); // Read player one start sequence
				currDisplayBoard = &playerOne.ownMap;
				currDisplayState = play; // allow player one to place ships
			}

			if (currMoveState == actionState) {
				currMoveState = idleState;

				/* Check for superposition */
				if (currCursorOrient == horizontalCursor) {
					if ( (playerOne.ownMap->horizontal_shipMap[ currCursorPosition[0] ][ currCursorPosition[1] ]) // space already contains ship
						|| ( (currShipType == doubleShip) // check if second half of double ship will superpose
							&& (playerOne.ownMap->horizontal_shipMap[ currCursorPosition[0] ][ currCursorPosition[1] + 1 ]) ) ) {
						break; // Law of superposition
					}
				} else { // cursor is vertical
					if (playerOne.ownMap->vertical_shipMap[ currCursorPosition[0] ][ currCursorPosition[1] ]) { // space already contains ship
						break; // law of superposition
					}
				} // Position is not occupied, safe to place

				placeShip(&playerOne);

				if ( (playerOne.singleBoatsRemaining == 3) & (playerOne.doubleBoatsRemaining == 2) ) { // All boats have been placed
					playerOneShipsPlaced = true;
				}
			}

			if (playerOneShipsPlaced) {
				currGameState = playerTwoStart;
				currDisplayState = scroll; // Toggle to display player two start sequence
			}
			break;

	/**
	 * Enter from "playerOneStart"
	 * Display message
	 * Player two places ships
	 * Move to "roundStart"
	 */
		case playerTwoStart:
			// enter on first pass after case "playerOneStart"
			if (currDisplayState = scroll) {
				outputMessage(); // Read player two start sequence
				currDisplayBoard = &playerTwo.ownMap;
				currDisplayState = play; // allow player two to place ships
			}

			if (currMoveState == actionState) {
				currMoveState = idleState;

				/* Check for superposition */
				if (currCursorOrient == horizontalCursor) {
					if ( (playerTwo.ownMap->horizontal_shipMap[ currCursorPosition[0] ][ currCursorPosition[1] ]) // space already contains ship
						|| ( (currShipType == doubleShip) // check if second half of double ship will superpose
							&& (playerTwo.ownMap->horizontal_shipMap[ currCursorPosition[0] ][ currCursorPosition[1] + 1 ]) ) ) {
						break; // law of superposition
					}
				} else { // cursor is vertical
					if (playerTwo.ownMap->vertical_shipMap[ currCursorPosition[0] ][ currCursorPosition[0] ]) { // space already contains ship
						break; // law of superposition
					}
				} // position is not occupied

				placeShip(&playerTwo);

				if ( (playerTwo.singleBoatsRemaining == 3) & (playerTwo.doubleBoatsRemaining == 2) ) { // All boats have been placed
					playerTwoShipsPlaced = true;
				}
			}

			if (playerTwoShipsPlaced) {
				currGameState = roundStart;
				currDisplayState = scroll;
			}
			break;

	/**
	 * Enter from "playerTwoStart" OR "playerTwoTurn"
	 * Display message
	 * Move to "playerOneTurn"
	 */
		case roundStart:
			numRounds++; // start at round 1 & increment
			currDisplayState = scroll; // this state only displays a message
			outputMessage(); // read roundStart sequence
			currGameState = playerOneTurn;
			break;

	/**
	 * Enter from "roundStart"
	 * Display message
	 * Player one plays
	 * Move to "playerTwoTurn" OR "endGame"
	 */
		case playerOneTurn:
			currMoveResult = noResult; // reset for turn

			// enter on first pass after case "roundStart"
			if (currDisplayState = scroll) {
				outputMessage(); // Read player one move sequence
				currDisplayBoard = &playerOne.opponentMap;
				currDisplayState = play; // allow player one to make move
			}

			if (currMoveState == actionState) {
				currMoveState = idleState;

				/* Check if position has been played */
				if (currCursorOrient == horizontalCursor) {
					if (playerOne.opponentMap->horizontal_LEDMap[ currCursorPosition[0] ][ currCursorPosition[1] ] == targetHit
					 || playerOne.opponentMap->horizontal_LEDMap[ currCursorPosition[0] ][ currCursorPosition[1] ] == targetMiss) {
						break; // the position cannot be played twice
					}
				} else {
					if (playerOne.opponentMap->vertical_LEDMap[ currCursorPosition[0] ][ currCursorPosition[1] ] == targetHit
					 || playerOne.opponentMap->vertical_LEDMap[ currCursorPosition[0] ][ currCursorPosition[1] ] == targetMiss) {
						break; // the position cannot be played twice
					}
				} // position is unplayed

				fireShot(&playerOne);
				currDisplayState = scroll; // player one's move is over
				outputMessage(); // display currMoveResult

				if (playerOne.numHits == 7) {
					currGameState = endGame; // Game ends
					break;
				}
				currGameState = playerTwoTurn; // Toggle to player two's move
			}
			break;

	/**
	 * Enter from "playerOneTurn"
	 * Display message
	 * Player two plays
	 * Move to "playerOneTurn" OR "endGame"
	 */
		case playerTwoTurn:
			currMoveResult = noResult; // reset for turn

			// enter on first pass after case "playerOneTurn"
			if (currDisplayState = scroll) {
				outputMessage(); // Read player two move sequence
				currDisplayBoard = &playerTwo.opponentMap;
				currDisplayState = play; // allow player two to make move
			}

			if (currMoveState == actionState) {
				currMoveState = idleState;

				/* Check if position has been played */
				if (currCursorOrient == horizontalCursor) {
					if ( !(playerTwo.opponentMap->horizontal_LEDMap[ currCursorPosition[0] ][ currCursorPosition[1] ] == off) ) {
						break; // the position cannot be played twice
					}
				} else {
					if ( !(playerTwo.opponentMap->vertical_LEDMap[ currCursorPosition[0] ][ currCursorPosition[1] ] == off) ) {
						break; // the position cannot be played twice
					}
				}

				fireShot(&playerTwo);
				currDisplayState = scroll; // player two's move is over
				outputMessage(); // display currMoveResult

				if (playerTwo.numHits == 7) {
					currGameState = endGame; // Game ends
					break;
				}
				currGameState = playerOneTurn; // Toggle to player one's move
			}
			break;

	/**
	 * Enter from "playerOneTurn" OR "playerTwoTurn"
	 * Display message
	 * Move to "title" (if player wants to playAgain)
	 */
		case endGame:
			if (playerOne.numHits == 7) {
				playerOne.numWins++;
			} else {
				playerTwo.numWins++;
			}

			outputMessage(); // Read winner sequence

			if (currMoveState == actionState) {
				currMoveState = idleState;
				playAgain = true;
			}

			if (playAgain) {
				resetGame();
				firstGame = false;
				currGameState = title;
			}
			break;
	}

	return;
}

/**
 * Coordinate output of messages or game boards to 7SEG display
 */
void outputMessage (void) {
	/* Prepare Game Board */
	bitMap cursorBoard = buildCursorBoard(potHorizontal_in, potVertical_in, currCursorOrient);
	*currDisplayBoard = compileBoard( &currDisplayBoard, cursorBoard ); // TODO make this work - somehow set the board to be displayed

	/* Prepare Message Output */
	switch (currGameState) {

		case title:
			/* Display Game Title */
			if (currDisplayState == scroll) {
				displayMessage(MessageTitle, sizeof(MessageTitle), scrollSpeed_norm, titleScroll_delay, displayHold_delay);

				if (!firstGame) {
					MessageP1Recap[21] = playerOne.numWins;
					displayMessage(MessageP1Recap, sizeof(MessageP1Recap), scrollSpeed_norm, recapScroll_delay, displayHold_delay);

					MessageP2Recap[21] = playerTwo.numWins;
					displayMessage(MessageP2Recap, sizeof(MessageP2Recap), scrollSpeed_norm, recapScroll_delay, displayHold_delay);
				} // TODO could add instruction message if it is the first game
			}

			break;

		case playerOneStart:
			/* Display Player One Start Screen*/
			if (currDisplayState == scroll) {
				displayMessage(MessageP1Start, sizeof(MessageP1Start), scrollSpeed_norm, playerStartScroll_delay, displayHold_delay);
			}

			break;

		case playerTwoStart:
			/* Display Player Two Start Screen*/
			if (currDisplayState == scroll) {
				displayMessage(MessageP2Start, sizeof(MessageP2Start), scrollSpeed_norm, playerStartScroll_delay, displayHold_delay);
			}

			break;

		case roundStart:
			if (currDisplayState == scroll) {
				displayRound_0th = numRounds & 0xF; // there will always be a 0th position round value to display
				/**
				 * The number of rounds is represented with a hexadecimal value. If the number of rounds played
				 * exceeds 15 (0xF), another digit must be included in the message.
				 */
				if(numRounds <= 0xF) {
					MessageRoundStart[16] = displayRound_0th; // update round number (0th position)
					displayMessage(MessageRoundStart, sizeof(MessageRoundStart), scrollSpeed_fast, roundScroll_delay, displayHold_delay);

				} else {
					displayRound_1st = (numRounds>>4) & 0xF; // include hex char 1 of numRounds if numRounds > 15
					MessageRoundStartEXT[17] = displayRound_0th; // update round number (0th position)
					MessageRoundStartEXT[16] = displayRound_1st; // update round number (1st position)
					displayMessage(MessageRoundStartEXT, sizeof(MessageRoundStartEXT), scrollSpeed_fast, extRoundScroll_delay, displayHold_delay);
				}
			}
			break;

		case playerOneTurn:
			GPIOD->ODR &= ~(1<<15); // turn of LED in bit 15 (represents player two's turn)
			GPIOD->ODR |= 1; // set LED in bit 0 high for player one's turn

			if (currDisplayState == scroll) {
				switch(currMoveResult) {

					case noResult: // move has not been made yet
						displayMessage(MessageP1Turn, sizeof(MessageP1Turn), scrollSpeed_norm, roundScroll_delay, displayHold_delay);
						break;

					case hitResult:
						displayMessage(MessageHit, sizeof(MessageHit), scrollSpeed_fast, moveScroll_delay, displayHold_delay);
						break;

					case missResult:
						displayMessage(MessageMiss, sizeof(MessageMiss), scrollSpeed_fast, moveScroll_delay, displayHold_delay);
						break;
				}
			}

			break;

		case playerTwoTurn:
			GPIOD->ODR &= ~(1); // turn of LED in bit 0 (represents player one's turn)
			GPIOD->ODR |= 1<<15; // set LED in bit 15 high for player two's turn

			if (currDisplayState == scroll) {

				switch(currMoveResult) {

					case noResult:
						displayMessage(MessageP2Turn, sizeof(MessageP2Turn), scrollSpeed_fast, roundScroll_delay, displayHold_delay);
						break;

					case hitResult:
						displayMessage(MessageHit, sizeof(MessageHit), scrollSpeed_fast, moveScroll_delay, displayHold_delay);
						break;

					case missResult:
						displayMessage(MessageMiss, sizeof(MessageMiss), scrollSpeed_fast, moveScroll_delay, displayHold_delay);
						break;
				}
			}

			break;

		/**
		 * Prompt players to restart the game and preserve win record with points? If we have the time
		 */
		case endGame:
			/* Display Game End Screen*/
			if (currDisplayState == scroll) {
				if (playerOne.numHits == 7) { // player one won
					displayMessage(MessageP1Wins, sizeof(MessageP1Wins), scrollSpeed_norm, winnerScroll_delay, displayHold_delay);

				} else { // player two won
					displayMessage(MessageP2Wins, sizeof(MessageP2Wins), scrollSpeed_norm, winnerScroll_delay, displayHold_delay);
				}
			}



			break;
	}
	return;
}

/**
 * Loop through input reading and game logic
 */
void game(void) {

	while(1) {
		  input(); // process inputs for logic
		  logic(); // coordinate game flow & toggle message or board display
	}
	return; // should not reach
}

	/////////////////////////////////
	//////////// Helpers ////////////
	/////////////////////////////////

/**
 * Set message to be scrolled on 7SEG display
 */
void displayMessage(char *messageNamePointer, int messageLength, int scrollSpeed, int messageDelay, int holdDelay) {
	Animate_On = 1; // Turn on animation so that interrupt displays message[]
	Message_Pointer = messageNamePointer;
	Save_Pointer = messageNamePointer;
	Message_Length = messageLength;
	Delay_msec = scrollSpeed;
	HAL_Delay(messageDelay);          // Delay to allow message to scroll
	Animate_On = 0;            // Stop scrolling message

	GPIOD->ODR |= 1<<7; // TODO delete after testing
	HAL_Delay(holdDelay);           // Delay to give space
	GPIOD->ODR &= ~(1<<7); // TODO delete after testing
	return;
}

/**
 * This method reads potentiometer values to set the cursor position.
 * It does this by checking the orientation of the cursor, then
 * moving through conditional checks to allow even ranges for the
 * potentiometers to sweep through for given positions
 */
bitMap* buildCursorBoard(int hPot, int vPot, enum cursorOrient orient) {

	static bitMap *cursorBoardPointer = &cursorBoard; // points to cursorBoard
	char row, col;

	//clear horizontal segments of cursorBoard
	for (row = 0; row < 3; row++) {
		for (col = 0; col < 8; col++) {
			cursorBoard.horizontal_LEDMap[row][col] = off;
		}
	}
	//clear vertical segments of cursorBoard
	for (row = 0; row < 2; row++) {
		for (col = 0; col < 16; col++) {
			cursorBoard.vertical_LEDMap[row][col] = off;
		}
	} row = col = 0;

	/* Determine column & row */
	if (orient == horizontalCursor) { // horizontal segments
		/**
		 * This block sets a single cursor blinking
		 */
		if (currShipType == singleShip // enables "superspeed mode" where program runs SOOO much faster in switch at PC0 is down
									   // because the loop enters one condition check earlier than if it is high
	    || (currGameState != playerOneStart && currGameState != playerTwoStart) ) {
			//Horizontal movement
				if (hPot < 512) { // Leftmost position = leftmost segment
					col = 0;
				} else if (hPot < 1024) {
					col = 1;
				} else if (hPot < 1536) {
					col = 2;
				} else if (hPot < 2048) { // Middle position = middle segment
					col = 3;
				} else if (hPot < 2560) {
					col = 4;
				} else if (hPot < 3072) {
					col = 5;
				} else if (hPot < 3584) {
					col = 6;
				} else { // Rightmost position = rightmost segment
					col = 7;
				}
			//Vertical movement
				if (vPot < 1365) { // Left position = top
					row = 0;
				} else if (vPot < 2730) { // middle position = middle
					row = 1;
				} else { // right position = bottom
					row = 2;
				}

			assignIndex_LEDState(cursorBoardPointer, row, col, horizontalCursor, cursorState);
		} else {
			/**
			 * This block  will be set to the position of col & col+1 for double ship functionality
			 * since it is
			 */
			//horizontal movement
				if (hPot < 585) { // Leftmost position = leftmost segment
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
				if (vPot < 1365) { // Left position = top
					row = 0;
				} else if (vPot < 2730) { // middle position = middle
					row = 1;
				} else { // right position = bottom
					row = 2;
				}

			assignIndex_LEDState(cursorBoardPointer, row, col, horizontalCursor, cursorState);
			assignIndex_LEDState(cursorBoardPointer, row, col+1, horizontalCursor, cursorState);
		}


	} else { // vertical segments
		if (currGameState == playerOneStart || currGameState == playerTwoStart) {
			if (currShipType == singleShip) {
			//horizontal movement
				if (hPot < 256) { // Leftmost position = leftmost segment
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
				if (vPot < 2048) { // Left position = top
					row = 0;
				} else { // right position = bottom
					row = 1;
				}

				assignIndex_LEDState(cursorBoardPointer, row, col, verticalCursor, cursorState);
			} else {
				/**
				 * For double ships, the cursor will be set to the position of row & row+1
				 */
			//horizontal movement functionality is unchanged
				if (hPot < 256) { // Leftmost position = leftmost segment
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

				assignIndex_LEDState(cursorBoardPointer, row, col, verticalCursor, cursorState);
				assignIndex_LEDState(cursorBoardPointer, row+1, col, verticalCursor, cursorState);
			}
		}
	}

	currCursorPosition[0] = row;
	currCursorPosition[1] = col;
	return cursorBoardPointer;
}

/**
 * Assign a specific location on a map with a given LED state
 */
void assignIndex_LEDState(char row, char col, bitMap *board, cursorOrient orient, LEDstate state) {

	if (orient == horizontalCursor) {
		board->horizontal_LEDMap[row][col] = state;
	} else {
		board->vertical_LEDMap[row][col] = state;
	}
	return;
}

/**
 * Produce combined game board with cursor
 */
bitMap compileBoard(bitMap *cursorPointer, bitMap *boardPointer) {
	bitMap comp = {
			.horizontal_LEDMap = { 0 },
			.vertical_LEDMap = { 0 }
	};

	/* compile horizontal */
	for (int row = 0; row < 3; row++) {
		for (int col = 0; col < 8; col++) {
			if (cursorPointer->horizontal_LEDMap[row][col] == blink) { // if the cursor is on the position
				comp.horizontal_LEDMap[row][col] = cursorState;;
			} else {
				comp.horizontal_LEDMap[row][col] = board->horizontal_LEDMap[row][col]; // set comp board position to that of board
			}
		}
	}
	/* compile vertical */
	for (int row = 0; row < 2; row++) {
		for (int col = 0; col < 16; col++) {
			if (cursorPointer->vertical_LEDMap[row][col] == blink) { // if the cursor is on the position
				comp.vertical_LEDMap[row][col] = cursorState;;
			} else {
				comp.vertical_LEDMap[row][col] = board->vertical_LEDMap[row][col]; // set comp board position to that of board
			}
		}
	}

	return comp;
}

/**
 * assign board positions with appropriate LED values
 */
void drawBoard(bitMap *board) {

	char seg[8] = { 0 };
	bool dimOn = (timer % 8 == 0); // sets PWM for dimming
	bool blinkOn = ((timer/250) % 2 == 0); // sets PWM for blinking

	/* set the horizontal segments */
	for (int i = 0; i < 8; i++)	{

		// set the top segments
		switch(board->horizontal_LEDMap[0][i]) {
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
		switch(board->horizontal_LEDMap[1][i]) {
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
		switch(board->horizontal_LEDMap[2][i]) {
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
		/* Left */
		// set the top left segments
		switch(board->vertical_LEDMap[0][i]) {
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
		switch(board->vertical_LEDMap[1][i]) {
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
		switch(board->vertical_LEDMap[0][i]) {
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
		switch(board->vertical_LEDMap[1][i]) {
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

/**
 * Check validity of ship positions and update player->owmMap->(h & v)_shipMap. Also update doubleBoatProperties[][]
 * and remaining ship count
 */
void placeShip(player *playerPointer) {
	if (currCursorOrient == horizontalCursor) { /*** Horizontal ***/

		if (currShipType == singleShip) { // place single ship
			playerPointer->ownMap->horizontal_shipMap[ currCursorPosition[0] ][ currCursorPosition [1] ] = hasShip;
			playerPointer->singleBoatsRemaining++; // playerPointer has placed one more single boat
		} else { // place double ship
			if (playerPointer->doubleBoatsRemaining == 0) { // set first boat properties

				playerPointer->doubleBoatProperties[0][0] = 0; // orientation is horizontal
				playerPointer->doubleBoatProperties[0][1] = currCursorPosition[0]; // set row to currCursorPosition row
				playerPointer->doubleBoatProperties[0][2] = currCursorPosition[1]; // set col to currCursorPosition col
			} else if (playerPointer->doubleBoatsRemaining == 1) { // set second boat properties

				playerPointer->doubleBoatProperties[1][0] = 0; // orientation is horizontal
				playerPointer->doubleBoatProperties[1][1] = currCursorPosition[0]; // set row to currCursorPosition row
				playerPointer->doubleBoatProperties[1][2] = currCursorPosition[1]; // set col to currCursorPosition col
			}
			playerPointer->ownMap->horizontal_shipMap[ currCursorPosition[0] ][ currCursorPosition [1] ] = hasShip; // first half of double boat
			playerPointer->ownMap->horizontal_shipMap[ currCursorPosition[0] ][ currCursorPosition [1] + 1 ] = hasShip; // second half of double boat
			playerPointer->doubleBoatsRemaining++; // player has placed one more double boat
		}

	} else { /*** Vertical ***/

		if (currShipType == singleShip) { // place single ship
			playerPointer->ownMap->vertical_shipMap[ currCursorPosition[0] ][ currCursorPosition [1] ] = hasShip;
			playerPointer->singleBoatsRemaining++; // player has placed one more single boat
		} else { // place double ship
			if (playerPointer->doubleBoatsRemaining == 0) { // set first boat properties

				playerPointer->doubleBoatProperties[0][0] = 1; // orientation is vertical
				playerPointer->doubleBoatProperties[0][1] = currCursorPosition[0]; // set row to currCursorPosition row
				playerPointer->doubleBoatProperties[0][2] = currCursorPosition[1]; // set col to currCursorPosition col
			} else if (playerPointer->doubleBoatsRemaining == 1) { // set second boat properties

				playerPointer->doubleBoatProperties[1][0] = 1; // orientation is vertical
				playerPointer->doubleBoatProperties[1][1] = currCursorPosition[0]; // set row to currCursorPosition row
				playerPointer->doubleBoatProperties[1][2] = currCursorPosition[1]; // set col to currCursorPosition col
			}
			playerPointer->ownMap->vertical_shipMap[ currCursorPosition[0] ][ currCursorPosition [1] ] = hasShip; // first half of double boat
			playerPointer->ownMap->vertical_shipMap[ currCursorPosition[0] + 1 ][ currCursorPosition [1] ] = hasShip; // second half of double boat
			playerPointer->doubleBoatsRemaining++; // player has placed one more double boat
		}

	}
	return;
}

/**
 * Determine currMoveResult and update player->ownMap and player->numHits
 */
void fireShot(player *playerPointer) {
	/* Determine currMoveResult */
	if (currCursorOrient == horizontalCursor) { // check cursor position against horizontal segments
		if (playerPointer->opponentMap->horizontal_shipMap[ currCursorPosition[0] ][ currCursorPosition[1] ]) { // position hasShip
			playerPointer->ownMap->horizontal_LEDMap[ currCursorPosition[0] ][ currCursorPosition[1] ] = targetHit; // bright
			currMoveResult = hitResult;
			playerPointer->numHits++;

		} else { // position isEmpty
			playerPointer->ownMap->horizontal_LEDMap[ currCursorPosition[0] ][ currCursorPosition[1] ] = targetMiss; // dim
			currMoveResult = missResult;
		}
	} else { // check cursor position against vertical segments
		if (playerPointer->opponentMap->vertical_shipMap[ currCursorPosition[0] ][ currCursorPosition[1] ]) { // position hasShip
			playerPointer->ownMap->vertical_LEDMap[ currCursorPosition[0] ][ currCursorPosition[1] ] = targetHit; // bright
			currMoveResult = hitResult;
			playerPointer->numHits++;
		} else { // position isEmpty
			playerPointer->ownMap->vertical_LEDMap[ currCursorPosition[0] ][ currCursorPosition[1] ] = targetMiss; // dim
			currMoveResult = missResult;
		}
	}

	/* Update game board */
	assignIndex_LEDState(currCursorPosition[0], currCursorPosition[1], &playerPointer->opponentMap, currCursorOrient, currMoveResult);

	return;
}

/**
 * Clear game data (except numWins)
 */
void resetGame(void) {
	int row, col; // Used to iterate through arrays

	/*** Player One ***/
	playerOne.singleBoatsRemaining = 0;
	playerOne.doubleBoatsRemaining = 0;
	playerOne.numHits = 0;

	for (row = 0; row < 2; row++) {
		for (col = 0; col < 3; col++) {
			playerOne.doubleBoatProperties[row][col] = 0; // clear all double boat properties
		}
	}
		/* Clear contents of ownMap */
	//Clear Horizontal
	for (row = 0; row < 3; row++) {
		for (col = 0; col < 8; col++) {
			playerOne.ownMap->horizontal_LEDMap[row][col] = off; // turn off all LEDs
			playerOne.ownMap->horizontal_shipMap[row][col] = isEmpty; // clear all ships
		}
	}
	//Clear Vertical
	for (row = 0; row < 2; row++) {
		for (col = 0; col < 16; col++) {
			playerOne.ownMap->vertical_LEDMap[row][col] = off; // turn off all LEDs
			playerOne.ownMap->vertical_shipMap[row][col] = isEmpty; // clear all ships
		}
	}

	/*** Player Two ***/
	playerTwo.singleBoatsRemaining = 0;
	playerTwo.doubleBoatsRemaining = 0;
	playerTwo.numHits = 0;

	for (row = 0; row < 2; row++) {
		for (col = 0; col < 3; col++) {
			playerTwo.doubleBoatProperties[row][col] = 0; // clear all double boat properties
		}
	}
		/* Clear contents of ownMap */
	//Clear Horizontal
	for (row = 0; row < 3; row++) {
		for (col = 0; col < 8; col++) {
			playerTwo.ownMap->horizontal_LEDMap[row][col] = off; // turn off all LEDs
			playerTwo.ownMap->horizontal_shipMap[row][col] = isEmpty; // clear all ships
		}
	}
	//Clear Vertical
	for (row = 0; row < 2; row++) {
		for (col = 0; col < 16; col++) {
			playerTwo.ownMap->vertical_LEDMap[row][col] = off; // turn off all LEDs
			playerTwo.ownMap->vertical_shipMap[row][col] = isEmpty; // clear all ships
		}
	}
}

