#include "battleship.h"

// data
int switches_in = 0;
int leds_out = 0;


static int timer = 0;

// function definitions
int input( void )
{

	switches_in = GPIOC->IDR;

	return 0;
}

int logic ( void )
{

	// process raw inputs
//	leds_out = ~switches_in;
	leds_out = timer >> 9;

	// game logic
	  for (int i=0 ; i<8 ; i++)      // Clear the display
	  {
		  Seven_Segment_Digit(i,SPACE,0);
	  }

	  if ( timer % 5 == 0 ){
		  Seven_Segment_Digit(7,CHAR_A,0);
	  }

	  if ( (timer / 500) % 2 == 0 ){
		  Seven_Segment_Digit(6,CHAR_A,0);
	  }
	  Seven_Segment_Digit(5,CHAR_A,0);

	// prepare outputs
	// ...

	return 0;
}

int output ( void )
{

	GPIOD->ODR = leds_out << 1;
	return 0;
}

int game( void )
{

	while(1)
	{
		  input();
		  logic();
		  output();
	}
}

int handle_interrupts ( void )
{


	timer++;
}
