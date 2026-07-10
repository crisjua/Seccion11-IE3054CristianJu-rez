/*
 * Laboratorio1.c
 *
 * Created: 9/7/2026 17:39:21
 * Author : Cristian Juárez
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "Display/Display.h"

//variables globales
volatile uint8_t estado = 0;
volatile uint8_t jugador1 = 0;
volatile uint8_t jugador2 = 0;

void configurarInterrupciones(void);
void actualizarContadores(void);

int main(void)
{
	//Configuracion inicial
	initDislayPorts();
	DDRB = 0x0F;
	DDRC = 0x0F; 
	
	//Habilitar pull ups para botones
	PORTB |= (1<< PB4);
	PORTC |= (1<< PC4) | (1<< PC5);
	
	//Configurar Interrupciones 
	configurarInterrupciones();
	sei();
	
    while(1) {
	    switch(estado) {
		    case 1: // Conteo regresivo
		    for(uint8_t i = 5; i != 255; i--) {
			    display(i);
			    _delay_ms(1000);
		    }
		    estado = 2; // Cambiar a estado carrera
		    break;
		    
		    case 2: // Carrera activa
		    actualizarContadores();
		    break;
		    
		    default: // Sistema inactivo
		    display(0);
		    PORTB &= 0xF0;
		    PORTC &= 0xF0;
		    break;
	    }
    }
    }

void actualizarContadores(void){
	PORTB = (PORTB & 0xF0) | (jugador1 & 0x0F);
	PORTC = (PORTC & 0xF0) | (jugador2 & 0x0F);
}


ISR(PCINT0_vect) { // Interrupción botón inicio (PB4)
	if(!(PINB & (1 << PB4)) && (estado == 0)) {
		estado = 1;
	}
}

ISR(PCINT1_vect) { // Interrupción botones jugadores
	if(estado == 2) {
		if(!(PINC & (1 << PC4))) { // Jugador 1 (PC4)
			jugador1 = (jugador1 < 15) ? jugador1 + 1 : 0;
			_delay_ms(200);
		}
		if(!(PINC & (1 << PC5))) { // Jugador 2 (PC5)
			jugador2 = (jugador2 < 15) ? jugador2 + 1 : 0;
			_delay_ms(200);
		}
		actualizarContadores();
	}
}

void configurarInterrupciones(void){
	PCICR |= (1<<PCIE0) | (1<<PCIE1);
	PCMSK0 |= (1<<PCINT4);
	PCMSK1 |= (1<<PCINT12) | (1<<PCINT13);
}