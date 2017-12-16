#pragma once
#include <avr/io.h>
#include <inttypes.h>

#ifndef UART_BAUD
#define UART_BAUD  9600
#endif
//initialise uart0
void initUart();
//sends a single byte and waits to receive N bytes
void serialGet(uint8_t send_byte, uint8_t recv_count, uint8_t* buffer);

void initUart(){
	UBRR0H = (F_CPU/(UART_BAUD*16L)-1) >> 8;
	UBRR0L = (F_CPU/(UART_BAUD*16L)-1);
	UCSR0B = _BV(RXEN0) | _BV(TXEN0);
	UCSR0C = _BV(UCSZ00) | _BV(UCSZ01);
}

void serialGet(uint8_t send_byte, uint8_t recv_count, uint8_t* buffer){
	while (!(UCSR0A & _BV(UDRE0)));	//Wait for serial to be free
	UDR0 = send_byte;		//Send serial byte
	uint8_t i;
	for(i=0; i<recv_count; i++){
		while(!(UCSR0A & _BV(RXC0)));	//Wait for received data
		buffer[i] = UDR0;
	}		
}