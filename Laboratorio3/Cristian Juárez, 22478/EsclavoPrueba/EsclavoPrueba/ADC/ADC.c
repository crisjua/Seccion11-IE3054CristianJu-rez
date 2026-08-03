//***************************************************************************
// Universidad del Valle de Guatemala
// IE3054: Electrónica Digital 2
// Autores: Diego Cardona 21933 & Cristian Juárez 22478
// Proyecto: Laboratorio 3, Comunicación SPI
// Hardware: Atmega328P
// Created: 23/07/2026 17:51:20
//***************************************************************************
// Laboratorio 3 - Comunicación SPI
// Librería ADC para Esclavo archivo include .c
//***************************************************************************

#include "ADC.h"

void ADC_init(void) {
	ADMUX = (1 << REFS0); // referencia AVcc
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1); // habilitar ADC, prescaler 64
}

uint16_t ADC_read(uint8_t channel) {
	ADMUX = (ADMUX & 0xF0) | (channel & 0x0F); // seleccionar canal
	ADCSRA |= (1 << ADSC); // iniciar conversión
	while(ADCSRA & (1 << ADSC)); // esperar fin
	return ADC;
}
