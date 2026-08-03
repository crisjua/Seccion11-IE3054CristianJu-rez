//***************************************************************************
// Universidad del Valle de Guatemala
// IE3054: Electrónica Digital 2
// Autores: Diego Cardona 21933 & Cristian Juárez 22478
// Proyecto: Laboratorio 3, Comunicación SPI
// Hardware: Atmega328P
// Created: 23/07/2026 17:49:31
//***************************************************************************
// Laboratorio 3 - Comunicación SPI
// Código Principal
//***************************************************************************

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include "SPI/SPI.h"
#include "USART/USART.h"

void refreshLEDs(uint8_t value);

int main(void) {
	DDRC |= (1 << DDC0);    // SS en PC0
	PORTC |= (1 << PORTC0);

	// Configurar LEDs del maestro
	DDRB |= (1 << DDB0) | (1 << DDB1);
	DDRD |= 0xFC;

	initUART9600();
	SPI_init(SPI_MASTER, SPI_MSB_FIRST, SPI_CLOCK_IDLE_LOW, SPI_SAMPLE_ON_LEADING);

	writeTextUART("Sistema Maestro listo\r\n");
	writeTextUART("Menu:\r\n");
	writeTextUART("P - Leer potenciómetros del esclavo y mostrar en LEDs\r\n");
	writeTextUART("L - Enviar valor manual a LEDs (0-255)\r\n");

	char buffer[64];
	char input[8];
	uint8_t idx;

	while(1) {
		unsigned char c = read_UART();

		// --- Opción P: leer potenciómetros ---
		if(c == 'P' || c == 'p') {
			PORTC &= ~(1 << PORTC0);

			SPI_transmit(0x00); // dummy
			uint8_t pot1 = SPI_receive();

			SPI_transmit(0x00); // dummy
			uint8_t pot2 = SPI_receive();

			PORTC |= (1 << PORTC0);

			sprintf(buffer, "Pot1: %d  Pot2: %d\r\n", pot1, pot2);
			writeTextUART(buffer);

			// Mostrar pot1 en LEDs del maestro
			refreshLEDs(pot1);

			// Enviar pot1 al esclavo
			PORTC &= ~(1 << PORTC0);
			SPI_transmit(pot1);
			PORTC |= (1 << PORTC0);

			// Mostrar pot2 en LEDs del maestro
			refreshLEDs(pot2);

			// Enviar pot2 al esclavo
			PORTC &= ~(1 << PORTC0);
			SPI_transmit(pot2);
			PORTC |= (1 << PORTC0);
		}

		// --- Opción L: enviar valor manual ---
		else if(c == 'L' || c == 'l') {
			idx = 0;
			while(1) {
				unsigned char d = read_UART();
				if(d == '\r' || d == '\n') {
					input[idx] = '\0';
					break;
					} else if(d >= '0' && d <= '9') {
					if(idx < sizeof(input)-1) {
						input[idx++] = d;
					}
				}
			}

			uint8_t val = atoi(input);

			// Mostrar en LEDs del maestro
			refreshLEDs(val);

			// Enviar al esclavo
			PORTC &= ~(1 << PORTC0);
			SPI_transmit(val);
			PORTC |= (1 << PORTC0);

			sprintf(buffer, "Enviado: %d\r\n", val);
			writeTextUART(buffer);
		}
	}
}

void refreshLEDs(uint8_t value) {
	PORTB = (PORTB & 0xFC) | ((value >> 6) & 0x03); // PB0–PB1
	PORTD = (value << 2) & 0xFC;                    // PD2–PD7
}
