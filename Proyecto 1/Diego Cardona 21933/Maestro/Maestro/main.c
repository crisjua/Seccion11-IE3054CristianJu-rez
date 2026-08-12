/*
 * Maestro.c
 *
 * Created: 6/8/2026 12:42:29
 * Author : crist
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "I2C/I2C.h"
#include "LCD/LCD.h"
#include "USART/USART.h"

// Direcciones de esclavos
#define SLAVE_PESO  0x08
#define SLAVE_US    0x40

// Buffers
char PESO_CHAR[16];
char DISTANCIA_CHAR[16];

// Variables globales para tara
volatile uint16_t tara_offset = 0;
static uint8_t last_state = 0; // estado anterior del botón

// --- Inicialización del botón en D13 (PB5) ---
static void boton_init(void) {
	DDRB &= ~(1 << PB5);   // D13 como entrada
	PORTB |= (1 << PB5);   // pull-up interno activado
}

static uint8_t boton_presionado(void) {
	return !(PINB & (1 << PB5)); // devuelve 1 si está presionado
}

int main(void) {
	// Inicializa I2C, LCD y UART
	I2C_Master_Init(100000,1);   // 100 kHz
	initLCD8bits();
	initUART9600();
	boton_init();   // inicializa el botón de tara

	// Etiquetas en LCD
	LCD_Clear();
	LCD_Set_Cursor(1,1); LCD_Write_String("Pes:");
	LCD_Set_Cursor(9,1); LCD_Write_String("Dis:");

	while(1) {
		// ============================
		// Leer PESO del esclavo 0x08
		// ============================
		I2C_Master_Start();
		uint8_t sla_w = (SLAVE_PESO<<1) & 0xFE;
		if(I2C_Master_Write(sla_w)==1){
			I2C_Master_Write('C'); // comando opcional
		}
		I2C_Master_Stop();
		_delay_ms(5);

		I2C_Master_Start();
		uint8_t sla_r = (SLAVE_PESO<<1) | 0x01;
		if(I2C_Master_Write(sla_r)==1){
			uint8_t msb=0, lsb=0;
			I2C_Master_Read(&msb,1);
			I2C_Master_Read(&lsb,0);
			I2C_Master_Stop();

			uint16_t gramos = ((uint16_t)msb<<8)|lsb;

			// --- Tara con debounce e inmediato ---
			uint8_t current = boton_presionado();
			if (current && !last_state) {
				_delay_ms(50); // anti-rebote
				if (boton_presionado()) {
					tara_offset = gramos;   // guarda offset
					// Forzar pantalla a cero inmediatamente
					snprintf(PESO_CHAR,sizeof(PESO_CHAR),"%5u",0);
					LCD_Set_Cursor(1,2); LCD_Write_String(PESO_CHAR);
					LCD_Set_Cursor(6,2); LCD_Write_String("g");
					_delay_ms(500); // retardo para ver el cero y el servo reaccionar
				}
			}
			last_state = current;

			// Aplicar corrección de tara
			if (gramos >= tara_offset) {
				gramos = gramos - tara_offset;
				} else {
				gramos = 0;
			}

			// Mostrar valor corregido
			snprintf(PESO_CHAR,sizeof(PESO_CHAR),"%5u",gramos);
			LCD_Set_Cursor(1,2); LCD_Write_String(PESO_CHAR);
			LCD_Set_Cursor(6,2); LCD_Write_String("g");
		}

		// ============================
		// Leer DISTANCIA del esclavo 0x40
		// ============================
		I2C_Master_Start();
		sla_w = (SLAVE_US<<1) & 0xFE;
		if(I2C_Master_Write(sla_w)==1){
			I2C_Master_Write('C');
		}
		I2C_Master_Stop();
		_delay_ms(5);

		I2C_Master_Start();
		sla_r = (SLAVE_US<<1) | 0x01;
		if(I2C_Master_Write(sla_r)==1){
			uint8_t cm=0;
			I2C_Master_Read(&cm,0);
			I2C_Master_Stop();

			snprintf(DISTANCIA_CHAR,sizeof(DISTANCIA_CHAR),"%3u",cm);
			LCD_Set_Cursor(9,2); LCD_Write_String(DISTANCIA_CHAR);
			LCD_Set_Cursor(12,2); LCD_Write_String("cm");
		}

		_delay_ms(300);
	}
}
