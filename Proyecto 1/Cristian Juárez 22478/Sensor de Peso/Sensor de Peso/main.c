/*
 * Sensor de Peso.c
 *
 * Created: 6/8/2026 10:32:03
 * Author : crist
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <util/delay.h>
#include "HX711/hx711.h"
#include "I2C/I2C_Esclavo.h"

// Variables de I2C_Esclavo.c
extern volatile uint16_t tx_snap;
extern volatile uint8_t rx_buf[2];
extern volatile uint8_t cmd_ready;

// ================= SERVO (Timer1, PB1 = OC1A) =================
// 50 Hz, pulso 1ms(0°) - 2ms(180°), prescaler 8 -> tick = 0.5us
#define SERVO_MIN_TICKS 2000   // 1.0 ms
#define SERVO_MAX_TICKS 4000   // 2.0 ms

static void servo_init(void) {
	DDRB |= (1 << PB1);              // OC1A como salida
	TCCR1A = (1 << COM1A1) | (1 << WGM11);
	TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11); // Fast PWM, prescaler 8
	ICR1 = 39999;                    // 50 Hz
	OCR1A = SERVO_MIN_TICKS;         // arranca en 0°
}

static void servo_set_angle(uint8_t angulo) {
	if (angulo > 180) angulo = 180;
	uint16_t ticks = SERVO_MIN_TICKS +
	((uint32_t)(SERVO_MAX_TICKS - SERVO_MIN_TICKS) * angulo) / 180;
	OCR1A = ticks;
}

// ================= STEPPER (28BYJ-48 + ULN2003, 4 pines) =================
#define STEP_DDR   DDRD
#define STEP_PORT  PORTD
#define STEP_IN1   PD4
#define STEP_IN2   PD5
#define STEP_IN3   PD6
#define STEP_IN7   PD7   // 4to pin

static const uint8_t stepper_seq[8] = {
	0b0001, 0b0011, 0b0010, 0b0110,
	0b0100, 0b1100, 0b1000, 0b1001
};

static uint8_t stepper_idx = 0;
#define STEPS_POR_REV 4096UL

static void stepper_init(void) {
	STEP_DDR |= (1 << STEP_IN1) | (1 << STEP_IN2) | (1 << STEP_IN3) | (1 << STEP_IN7);
}

static void stepper_write(uint8_t patron) {
	if (patron & 0x1) STEP_PORT |= (1 << STEP_IN1); else STEP_PORT &= ~(1 << STEP_IN1);
	if (patron & 0x2) STEP_PORT |= (1 << STEP_IN2); else STEP_PORT &= ~(1 << STEP_IN2);
	if (patron & 0x4) STEP_PORT |= (1 << STEP_IN3); else STEP_PORT &= ~(1 << STEP_IN3);
	if (patron & 0x8) STEP_PORT |= (1 << STEP_IN7); else STEP_PORT &= ~(1 << STEP_IN7);
}

static void stepper_move_degrees(uint16_t grados) {
	if (grados > 360) grados = 360;
	uint32_t pasos = ((uint32_t)STEPS_POR_REV * grados) / 360UL;
	
	for (uint32_t i = 0; i < pasos; i++) {
		stepper_idx = (stepper_idx + 1) & 0x07;
		stepper_write(stepper_seq[stepper_idx]);
		_delay_ms(2);
	}
	stepper_write(0); // apaga bobinas
}

// ---------- UART ----------
void USART_Init(unsigned int ubrr) {
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	UCSR0B = (1<<TXEN0);
	UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);
}

void USART_Transmit(unsigned char data) {
	while (!(UCSR0A & (1<<UDRE0)));
	UDR0 = data;
}

void USART_SendString(const char* str) {
	while (*str) {
		USART_Transmit(*str++);
	}
}

// Procesa comando recibido por I2C (Servo o Stepper)
static void procesar_comando(void) {
	if (!cmd_ready) return;
	
	uint8_t cmd   = rx_buf[0];
	uint8_t valor = rx_buf[1];
	cmd_ready = 0;
	
	switch (cmd) {
		case 2: // Servo
		servo_set_angle(valor);
		break;
		case 3: // Stepper
		stepper_move_degrees(valor);
		break;
		default:
		break;
	}
}

// ---------- MAIN ----------
int main(void) {
	HX711_init();
	USART_Init(103); // 9600 baudios
	servo_init();
	stepper_init();
	_delay_ms(800);
	
	I2C_Slave_Init(0x08);
	sei();
	
	while (1) {
		// Procesar comandos entrantes
		procesar_comando();
		
		// Leer HX711
		cli();
		long raw = HX711_read_avg(4);
		sei();
		
		if (raw < 0) raw = 0;
		if (raw > 65535) raw = 65535;
		tx_snap = (uint16_t)raw;
		
		// Convertir a gramos (ajusta factor según calibración)
		float gramos = raw / 100.0; // ejemplo de factor de escala
		
		// Control del SG90 según peso
		uint8_t angulo = 0;
		if (gramos >= 32.0) {
			angulo = 180; // 100%
			} else if (gramos >= 19.2) {
			angulo = 90;  // 50%
			} else if (gramos >= 6.4) {
			angulo = 45;  // 25%
			} else {
			angulo = 0;   // apagado
		}
		servo_set_angle(angulo);
		
		// Debug por UART
		char buffer[20];
		ltoa(raw, buffer, 10);
		USART_SendString("Lectura HX711: ");
		USART_SendString(buffer);
		USART_SendString(" -> ");
		USART_SendString("Peso(g): ");
		char gbuf[10];
		dtostrf(gramos, 4, 1, gbuf);
		USART_SendString(gbuf);
		USART_SendString(" Servo: ");
		char abuf[5];
		itoa(angulo, abuf, 10);
		USART_SendString(abuf);
		USART_SendString("\r\n");
		
		_delay_ms(200);
	}
}
