//***************************************************************************
// Universidad del Valle de Guatemala
// IE3054: Electrónica Digital 2
// Autores: Diego Cardona 21933 & Cristian Juárez 22478
// Proyecto: Laboratorio 3, Comunicación SPI
// Hardware: Atmega328P
// Created: 23/07/2026 17:49:31
//***************************************************************************
// Laboratorio 3 - Comunicación SPI
// Código Principal, Esclavo
//***************************************************************************

#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include "ADC/ADC.h"
#include "SPI/SPI.h"

void refreshLEDs(uint8_t value);

int main(void) {
	// Configurar LEDs
	DDRB |= (1 << DDB0) | (1 << DDB1);
	DDRD |= 0xFC;

	// Inicializar ADC y SPI esclavo
	ADC_init();
	SPI_init(SPI_SLAVE, SPI_MSB_FIRST, SPI_CLOCK_IDLE_LOW, SPI_SAMPLE_ON_LEADING);
	SPI_enable_interrupt();
	sei();

	while(1) {
		// bucle vacío, todo se maneja en la ISR
	}
}

void refreshLEDs(uint8_t value) {
	PORTB = (PORTB & 0xFC) | ((value >> 6) & 0x03); // PB0–PB1
	PORTD = (value << 2) & 0xFC;                    // PD2–PD7
}

ISR(SPI_STC_vect) {
	uint8_t received = SPDR;

	// Si el maestro manda dummy (0x00), respondemos con potenciómetro
	if(received == 0x00) {
		static uint8_t toggle = 0;
		if(toggle == 0) {
			uint16_t pot1 = ADC_read(0); // leer ADC0
			SPDR = (uint8_t)(pot1 >> 2); // convertir a 8 bits
			refreshLEDs((uint8_t)(pot1 >> 2)); // mostrar también en LEDs
			toggle = 1;
			} else {
			uint16_t pot2 = ADC_read(1); // leer ADC1
			SPDR = (uint8_t)(pot2 >> 2);
			refreshLEDs((uint8_t)(pot2 >> 2));
			toggle = 0;
		}
		} else {
		// Si el maestro manda un valor real, mostrar en LEDs
		refreshLEDs(received);
		SPDR = received; // eco opcional
	}
}
