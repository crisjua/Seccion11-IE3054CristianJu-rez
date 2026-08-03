//***************************************************************************
// Universidad del Valle de Guatemala
// IE3054: Electrónica Digital 2
// Autores: Diego Cardona 21933 & Cristian Juárez 22478
// Proyecto: Laboratorio 3, Comunicación SPI
// Hardware: Atmega328P
// Created: 23/07/2026 17:51:20
//***************************************************************************
// Laboratorio 3 - Comunicación SPI
// Librería ADC para Esclavo archivo include .h
//***************************************************************************


#ifndef ADC_H
#define ADC_H

#include <avr/io.h>
#include <stdint.h>

void ADC_init(void);
uint16_t ADC_read(uint8_t channel);

#endif
