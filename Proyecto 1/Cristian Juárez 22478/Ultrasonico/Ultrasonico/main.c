/*
 * Ultrasonico.c
 *
 * Created: 5/8/2026 17:38:47
 * Author : crist
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "Ultrasonico/Ultrasonico.h"
#include "I2C/I2C.h"

#define IN1_DDR   DDRD
#define IN1_PORT  PORTD
#define IN1_PIN   PD2

#define IN2_DDR   DDRD
#define IN2_PORT  PORTD
#define IN2_PIN   PD3

#define LED_DDR   DDRC
#define LED_PORT  PORTC
#define LED_PIN   PC0

// Variables globales de I2C.c
extern volatile uint8_t I2C_SlaveData;
extern volatile uint8_t I2C_LastReceived;

// Marca de comando procesado
#define SIN_COMANDO 0xFF

// Modo de operación del motor
static uint8_t modo_manual = 0;   // 0 = automático, 1 = manual
static uint8_t estado_manual = 0; // 0 = apagado, 1 = encendido (solo aplica si modo_manual=1)

// Inicialización de pines
static inline void motor_init(void) {
	IN1_DDR |= (1 << IN1_PIN);
	IN2_DDR |= (1 << IN2_PIN);
}
static inline void motor_forward(void) {
	IN1_PORT |= (1 << IN1_PIN);
	IN2_PORT &= ~(1 << IN2_PIN);
}
static inline void motor_stop(void) {
	IN1_PORT &= ~(1 << IN1_PIN);
	IN2_PORT &= ~(1 << IN2_PIN);
}
static inline void led_init(void) { LED_DDR |= (1 << LED_PIN); }
static inline void led_on(void)   { LED_PORT |= (1 << LED_PIN); }
static inline void led_off(void)  { LED_PORT &= ~(1 << LED_PIN); }

// Procesa comandos entrantes desde el maestro
static void procesar_comando_entrante(void) {
	uint8_t cmd = I2C_LastReceived;
	if (cmd == SIN_COMANDO) return; // no hay nada nuevo
	
	if (cmd == 0) {
		modo_manual = 1;
		estado_manual = 0;
		} else if (cmd == 1) {
		modo_manual = 1;
		estado_manual = 1;
		} else if (cmd == 2) {
		modo_manual = 0; // vuelve a automático
	}
	
	I2C_LastReceived = SIN_COMANDO; // marcar como procesado
}

int main(void) {
	motor_init();
	led_init();
	
	// Inicializa esclavo I2C en dirección 0x40
	I2C_Slave_Init(0x40);
	I2C_LastReceived = SIN_COMANDO;
	
	while (1) {
		procesar_comando_entrante();
		
		if (modo_manual) {
			// ---------- MODO MANUAL ----------
			if (estado_manual) {
				motor_forward();
				led_on();
				} else {
				motor_stop();
				led_off();
			}
			_delay_ms(100);
			} else {
			// ---------- MODO AUTOMÁTICO ----------
			cli();
			int16_t d = ultrasonic_read_cm_blocking();
			sei();
			
			if (d > 0 && d > 27) {
				motor_forward(); // sigue subiendo
				led_off();
				} else if (d > 14 && d <= 27) {
				motor_stop();    // piso 1 alcanzado
				led_on();
				I2C_SlaveData = (uint8_t)d;
				} else if (d > 1 && d <= 14) {
				motor_stop();    // piso 2 alcanzado
				led_on();
				I2C_SlaveData = (uint8_t)d;
				} else if (d > 0 && d <= 1) {
				motor_stop();    // piso 3 alcanzado
				led_on();
				I2C_SlaveData = (uint8_t)d;
				} else {
				motor_stop();    // error de lectura
				led_off();
				I2C_SlaveData = 0;
			}
			
			_delay_ms(100);
		}
	}
}
