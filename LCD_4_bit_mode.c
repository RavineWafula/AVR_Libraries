#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>
#include "LCD_4_bit_mode.h"

# define F_CPU 8000000UL  // define CPU Frequency -> 8MHz

#define LCD_PORT_DIR   DDRD
#define LCD_PORT       PORTD

#define 	RS   PD0  // define Register Select pin
#define 	EN   PD1	// define Enable Signal pin

const int ADDRESSES[8] = {64, 72, 80, 88, 96, 104, 112, 120};

/* We use PORTD */

void Send_Pulse_through_EN(){
    LCD_PORT |= (1<<EN);
    _delay_us(1);
    LCD_PORT &= ~(1<<EN);
}

void LCD_initialization(){
    LCD_PORT_DIR = 0xFF;// PD0  to PD7 declared as output
     _delay_ms(20); 	// LCD Power ON delay always > 15ms
   
    LCD_Command(0x33);
    LCD_Command(0x32);
    LCD_Command(0x02);	// send for 4 bit initialization of LCD
    LCD_Command(0x28);  // to initialize LCD in 2 line 5*7 matrix in 4-bit mode
    LCD_Command(0x0C);  // Display on cursor off
    LCD_Command(0x06);  // Incremant cursor (shift cursor to right)
    LCD_Command(0x01);  // Clear display screen
    LCD_Command(0x08);  // sets cursor to (0,0)
    _delay_ms(2);
}

void LCD_Command(unsigned char cmnd){
    // sending upper nibble
    LCD_PORT = (LCD_PORT & 0x0F) | (cmnd & 0xF0);
    // When RS==0 -> Command register
    LCD_PORT &= ~(1<<RS);

    Send_Pulse_through_EN();
    _delay_us(200);

    // sending lower nibble
    LCD_PORT = (LCD_PORT & 0x0F) | (cmnd << 4);

    Send_Pulse_through_EN();
    _delay_ms(2);
}

// send string to LCD with xy position
void LCD_xy_String(char row, char pos, char *str){
    if(row==0 && pos<16){
        // Command of first row and required position
        LCD_Command((pos & 0x0F) | 0x80);
    }
    else if (row==1 && pos <16){
        // Command of second row and required position
        LCD_Command((pos & 0x0F) | 0xC0);
    }
    LCD_Write_String(str);
}

void LCD_Clear(){
    // Clear display
    LCD_Command(0x01);
    _delay_ms(2);
    // Cursor at home position
    LCD_Command(0x80);
}

void LCD_Write_Char(unsigned char data){
    // sending upper nibble
    LCD_PORT = (LCD_PORT & 0x0F) | (data & 0xF0);
    // When RS==1 -> Data register
    LCD_PORT |= (1<<RS);

    Send_Pulse_through_EN();
    _delay_us(200);

    // sending lower nibble
    LCD_PORT = (LCD_PORT & 0x0F) | (data << 4);
    Send_Pulse_through_EN();
    _delay_ms(2);
}

void LCD_Write_String(char *str){
    int i;
    for(i=0; str[i]!=0; i++){
        LCD_Write_Char(str[i]);
    }
}

void setCursor(int row, int col){
    switch (row){
        case 0:
            switch (col){
                case 0: LCD_Command(0x80); break;
                case 1: LCD_Command(0x81); break;
                case 2: LCD_Command(0x82); break;
                case 3: LCD_Command(0x83); break;
                case 4: LCD_Command(0x84); break;
                case 5: LCD_Command(0x85); break;
                case 6: LCD_Command(0x86); break;
                case 7: LCD_Command(0x87); break;
                case 8: LCD_Command(0x88); break;
                case 9: LCD_Command(0x89); break;
                case 10: LCD_Command(0x8A); break;
                case 11: LCD_Command(0x8B); break;
                case 12: LCD_Command(0x8C); break;
                case 13: LCD_Command(0x8D); break;
                case 14: LCD_Command(0x8E); break;
                case 15: LCD_Command(0x8F); break;
                default: break;
            }
            break;
        case 1:
            switch (col){
                case 0: LCD_Command(0xC0); break;
                case 1: LCD_Command(0xC1); break;
                case 2: LCD_Command(0xC2); break;
                case 3: LCD_Command(0xC3); break;
                case 4: LCD_Command(0xC4); break;
                case 5: LCD_Command(0xC5); break;
                case 6: LCD_Command(0xC6); break;
                case 7: LCD_Command(0xC7); break;
                case 8: LCD_Command(0xC8); break;
                case 9: LCD_Command(0xC9); break;
                case 10: LCD_Command(0xCA); break;
                case 11: LCD_Command(0xCB); break;
                case 12: LCD_Command(0xCC); break;
                case 13: LCD_Command(0xCD); break;
                case 14: LCD_Command(0xCE); break;
                case 15: LCD_Command(0xCF); break;
                default: break;
            }
            break;
        default: break;
    }   
}
// returns cursor to (0,0) position
void home(){
    LCD_Command(0x02);
}
// Shows cursor as an underscore
void underscore(){
    LCD_Command(0x0E);
}
// Hide cursor
void hideCursor(){
    LCD_Command(0x0C);
}
// Shows cursor as a blinking black spot
void blink(){
    LCD_Command(0x0F);
}
// Display ON with cursor OFF
void display(){
    LCD_Command(0x0C);
}
// Display OFF
void noDisplay(){
    LCD_Command(0x08);
}
/* Scrolls the contents of the display(text and cursor) one
   space to the left */
void scrollDisplayLeft(){
    LCD_Command(0x18);
}
/* Scrolls the contents of the display(text and cursor) one
   space to the right */
void scrollDisplayRight(){
    LCD_Command(0x1C);
}
/* Causes each character output to the display to push previous
   characters over by one space in right to left direction*/
void autoscroll(){
    LCD_Command(0x07);
}
// Turns off automatic scrolling of the LCD
void noAutoscroll(){
    LCD_Command(0x06);
}

/* Takes two arguments: 
       - the 1st is a number between 0-7 which maps to eight base address
       - the 2nd is an array of 8 integers, each integer is formed by 5 bits
          which determine the pixels in the row same as the index of that
          integer
    Before printing the character, one must set cursor else cursor won't get 
    printed
*/
void createChar(int num, unsigned int *charArray){
    int i=0;
    LCD_Command(ADDRESSES[num]);

    while(i<8){
        LCD_Write_Char(charArray[i]);
        i++;
    }
}
/* To print created character, one can use LCD_Write_Char() function
   Just pass a number between 0-7 as an argument
   e.g LCD_Write_Char(5);  not '5' XXX

*/
