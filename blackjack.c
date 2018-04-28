/*
Author: Barrett Stolzman
Date: 4/27/18
*/

#include <stdlib.h>
#include <time.h>
//function prototypes
void LCD_cursor( int, int );
void LCD_text( char * );
void LCD_clear();
void LCD_cursor_off( void );
void display_hex(int, int);
int random_generation(int);

int main(void)
{
  volatile int * key_adr = (int *) 0x10000050;  // key address
  volatile int * sw_adr = (int *) 0x10000040;	// switch address
  //lcd messages
  char p1_text[40] = "Player one wins!\0";  
  char p2_text[40] = "Dealer wins!\0";
  char start_1[40] = "Blackjack(kinda)";
  char start_2[40] = "Press Key 3";
  char push[40] = "Push!";
  char reset1[40] = "Press Key[1]";
  char reset2[40] = "to continue....";
	
  //game loop
  while(1)
  {
	int count = 0; //count variable used to seed random number generation
	int p2_sum = 0; //dealer sum
	int p1_sum = 0; //player sum
	LCD_clear(); //clear display
	LCD_cursor(0,0); //set lcd cursor to top left
	LCD_text(start_1); //print message
	LCD_cursor(0,1); //set lcd cursor to bottom left
	LCD_text(start_2); //print message
	int key_val = *(key_adr); //prime key value
	
	while(key_val != 0x8) //increment counter while waiting for user to start the game
	{
		count++;
		key_val = *(key_adr);
	}
	
	LCD_clear();	
	
	srand(count); //seed random number with count
	
	for(int i = 0; i < 2; i++) //initializes player 1 cards
	{
		p1_sum += (rand() % 10 + 2); //adds random number between 2 and 10	
	}
	
	p2_sum += rand() % 10 + 2;
	
	display_hex(p1_sum,p2_sum); //display current scores
	
	
	while(key_val != 0x4 && p1_sum < 21) //player card loop
	{
		if (key_val == 0x2) 
		{
			int x = 0; //used for random number generation
			
			while(key_val == 0x2)
			{
				x++;
				key_val = *(key_adr);
			}
			
			for(int i = 0; i < 100000; i++);
			
			x = random_generation(x);
			
			p1_sum += x;
			
			display_hex(p1_sum, p2_sum);
		}
		key_val = *(key_adr);
	}
	
	p2_sum += rand() % 10 + 2;
	
	int y = 0;
	while(key_val == 0x4)
	{
		y++;
		key_val = *(key_adr);
	}
	
		
	while(p1_sum < 21 && p2_sum <= 17 || p2_sum < p1_sum)
	{
		p2_sum += random_generation(y);
	}
	
	display_hex(p1_sum,p2_sum);
	
	if(p1_sum == p2_sum)
	{
		for(int i = 0; i < 10000; i++)
		{
			LCD_cursor(0,0);
			LCD_text(push);
			LCD_cursor_off();
		}	
	}
	else if(p1_sum > 21 || p2_sum > p1_sum && p2_sum <= 21 && p1_sum != 21)
	{
		for(int i = 0; i < 1000; i++)
		{
			LCD_cursor(0,0);
			LCD_text(p2_text);
			LCD_cursor_off();
		}
	}
	else
	{
		for(int i = 0; i < 5000; i++)
		{
			LCD_cursor(0,0);
			LCD_text(p1_text);
			LCD_cursor_off();			
		}

	}
	
	
	LCD_clear();
	LCD_cursor(0,0);
	LCD_text(reset1);
	LCD_cursor(0,1);
	LCD_text(reset2);	
	LCD_cursor_off();	
	
	key_val = *(key_adr);
	
	while(key_val != 0x2)
	{
		key_val = *(key_adr);
	}
  }
}

//Initializes cursor on lcd
void LCD_cursor(int x, int y)
{
	volatile char * LCD_display_ptr = (char *) 0x10003050;	// 16x2 character display
	char instruction;

	instruction = x;
	if (y != 0) instruction |= 0x40;				// set bit 6 for bottom row
	instruction |= 0x80;								// need to set bit 7 to set the cursor location
	*(LCD_display_ptr) = instruction;			// write to the LCD instruction register
}

//Displays text on lcd display
void LCD_text(char * text_ptr)
{
	volatile char * LCD_display_ptr = (char *) 0x10003050;	// 16x2 character display

	while ( *(text_ptr) )
	{
		*(LCD_display_ptr + 1) = *(text_ptr);	// write to the LCD data register
		++text_ptr;
	}
}

//clears LCD display
void LCD_clear()
{
	char clear[40] = "                ";
	LCD_cursor(0,0);
	LCD_text(clear);
	LCD_cursor(0,1);
	LCD_text(clear);
	LCD_cursor_off();
}

//Turns off flashing cursor
void LCD_cursor_off(void)
{
	volatile char * LCD_display_ptr = (char *) 0x10003050;	// 16x2 character display
	*(LCD_display_ptr) = 0x0C;											// turn off the LCD cursor
}

//Displays player scores
void display_hex(int p1, int p2)
{
	volatile int * hex_07_adr = (int *) 0x10000030;
	unsigned char	seven_seg_decode_table[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x67}; //segment decoder
	
	char p1t = seven_seg_decode_table[p1 / 10];
	char p1o = seven_seg_decode_table[p1 % 10];
	char p2t = seven_seg_decode_table[p2 / 10];
	char p2o = seven_seg_decode_table[p2 % 10];
	
	*(hex_07_adr) = p1t << 24 | p1o << 16 | p2t << 8 | p2o;
}

int random_generation(int x)
{
	srand(x);
	return (rand() % 10 + 2);
}
