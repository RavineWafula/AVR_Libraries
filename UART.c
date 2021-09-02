#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <util/delay.h>
#include "UART.h"

#ifndef F_CPU 
#define F_CPU 8000000UL
#endif

#define BAUDRATE 9600
#define PRESCALE ((F_CPU/(BAUDRATE*16UL))-1)

void UART_initialization(long BAUDRATE){
    // set the baud rate
    UBRR0L = PRESCALE;       // lower 8 bits
    UBRR0H = (PRESCALE>>8);  // upper 8 bits
    // enable reciver and transmitter
    UCSR0B = (1<<RXEN0) | (1<<TXEN0);
    // set frame format: 8 data, 2 stop bit
    UCSR0C = (1<<USBS0) | (3<<UCSZ00);
}

void USART_TxChar(unsigned char data){
    // wait for empty transmit buffer
    while(!(UCSR0A & (1<<UDRE0)));

    // put data into buffer. this sends the data
    UDR0 = data;
}

unsigned char USART_RxChar(){
    // wait for data to be received
    while(!(UCSR0A & (1<<RXC0)));

    // get and return received data from buffer
    return UDR0;
}

/* flushing the receive buffer. If the buffer has to be flushed
   during normal operation, due to for instance an error condition,
   read the UDRn I/O location until the RXCn Flag is cleared 
 */
void USART_Flush(){
    unsigned char dummy;
    while(UCSR0A & (1<<RXC0));
    dummy = UDR0;
}

void USART_SendString(char *str){
    int i = 0;
    while(str[i]!=0){
        USART_TxChar(str[i]);
        i++;
    }
}
