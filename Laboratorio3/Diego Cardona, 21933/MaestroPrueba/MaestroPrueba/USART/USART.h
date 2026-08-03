//***************************************************************************
// Universidad del Valle de Guatemala
// IE3054: Electrónica Digital 2
// Autores: Diego Cardona 21933 & Cristian Juárez 22478
// Proyecto: Laboratorio 3, Comunicación SPI
// Hardware: Atmega328P
// Created: 23/07/2026 17:51:20
//***************************************************************************
// Laboratorio 3 - Comunicación SPI
// Librería UART para Maestro archivo include .h
//***************************************************************************

#ifndef USART_H
#define USART_H

#include <avr/io.h>
#include <stdint.h>

void initUART9600(void);
void writeUART(char character);
void writeTextUART(char* text);
unsigned char read_UART(void);

#endif
