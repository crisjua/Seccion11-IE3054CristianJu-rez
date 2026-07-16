/*
 * Display.c
 *
 * Created: 9/7/2026 17:39:53
 *  Author: Cristian Juarez
 */ 
#include <avr/io.h>
#include "Display.h"


void initDisplayPorts(void) {  // Nombre corregido
	DDRD = 0xFF;    // PORTD como salida
	PORTD = 0x00;   // Apagar todos los segmentos
	UCSR0B = 0;     // Deshabilitar UART para usar los pines normales
}

void display(uint8_t digit) {
	const uint8_t segments[] = {
		0x3F, 0x06, 0x5B, 0x4F,  // 0-3
		0x66, 0x6D, 0x7D, 0x07,  // 4-7
		0x7F, 0x6F, 0x77, 0x7C,  // 8-11
		0x39, 0x5E, 0x79, 0x71   // 12-15
	};
	
	if (digit < 16) {
		PORTD = segments[digit];
	}
}