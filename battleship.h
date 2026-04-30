#ifndef BATTLESHIP_H
#define BATTLESHIP_H

#include "main.h"
#include "usb_host.h"
#include "seg7.h"
#include <stdbool.h>

/*** Constant Macros ***/
#define PUSHBUTTON_DELAY 300
#define SCROLLSPEED_NORM 150
#define SCROLLSPEED_FAST 100
#define DISPLAYHOLD_DELAY 1000
#define TITLESCROLL_DELAY 3300
#define RECAPSCROLL_DELAY 8000
#define PLAYERSTARTSCROLL_DELAY 5000
#define ROUNDSCROLL_DELAY 2000
#define EXTROUNDSCROLL_DELAY 2100
#define MOVESCROLL_DELAY 1800
#define WINNERSCROLL_DELAY 5000
#define ENDGAMESCROLL_DELAY 10000
//TODO add delay for "Play Again"

/*** Enumerated variables ***/
typedef enum {
    title = 0,
    roundStart = 1,
    playerOneStart = 2,
    playerTwoStart = 3,
    playerOneTurn = 4,
    playerTwoTurn = 5,
    endGame = 6
} gameState; /* All states the game can be in */


typedef enum { // TODO could be cool to have "sunk" state
	off = 0,
	targetMiss = 1, // Dim
	targetHit = 2, // Bright
	blink = 3 // Cursor
} LEDstate; /* All states a board position can be in */

typedef enum {
	isEmpty = false,
	hasShip = true
} positionState; /* Contents of a board position */

typedef enum {
	horizontalCursor = 0,
	verticalCursor = 1
} cursorOrient; /* Orientation of the cursor */

typedef enum {
	singleShip = 0,
	doubleShip = 1
} shipType; /* Type of ship */

typedef enum {
	idleState = 0,
	actionState = 1
} moveState; /* State of current/most recent move */

typedef enum {
	noResult = 0,
	missResult = 1,
	hitResult = 2
} moveResult; /* Result of current/most recent move */

typedef enum {
	scroll = 0, // Display message
	play = 1 // Display game board
} displayState; /* State of 7SEG display */

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

/*** Function Declarations ***/
void game (void);
void handle_interrupts (void);

#endif /* BATTLESHIP_H */
