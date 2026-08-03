//***************************************************************************
// Universidad del Valle de Guatemala
// IE3054: Electrónica Digital 2
// Autores: Diego Cardona 21933 & Cristian Juárez 22478
// Proyecto: Laboratorio 3, Comunicación SPI
// Hardware: Atmega328P
// Created: 23/07/2026 17:51:10
//***************************************************************************
// Laboratorio 3 - Comunicación SPI
// Librería UART para Maestro archivo .c
//***************************************************************************

#include "USART.h"

void initUART9600(void) {
	// Configurar pines
	DDRD |= (1 << DDD1);   // TX como salida
	DDRD &= ~(1 << DDD0);  // RX como entrada
	
	// Configurar UART
	UCSR0A = (1 << U2X0);  // Modo rápido (doble velocidad)
	UCSR0B = (1 << RXEN0) | (1 << TXEN0); // Habilitar TX y RX
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8 bits de datos
	
	UBRR0 = 207; // 9600 bauds con F_CPU=16MHz y U2X0=1
}

void writeUART(char character) {
	while(!(UCSR0A & (1 << UDRE0))); // espera buffer libre
	UDR0 = character;
}

void writeTextUART(char* text) {
	for(uint8_t i = 0; text[i] != '\0'; i++) {
		writeUART(text[i]);
	}
}

unsigned char read_UART(void) {
	while(!(UCSR0A & (1 << RXC0))); // ?? espera hasta que llegue un dato
	return UDR0;                     // devuelve el carácter recibido
}
