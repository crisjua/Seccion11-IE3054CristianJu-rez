/*
 * I2C_Esclavo.c
 *
 * Created: 6/8/2026 12:21:56
 *  Author: crist
 */ 

#include "I2C_Esclavo.h"

// Variables globales (ya existian)
volatile uint16_t tx_snap = 0;
volatile uint8_t tx_idx = 0;

// Variables NUEVAS para recibir comandos
volatile uint8_t rx_buf[2] = {0, 0};
volatile uint8_t rx_idx = 0;
volatile uint8_t cmd_ready = 0;

void I2C_Slave_Init(uint8_t addr) {
	TWSR = 0x00;               // prescaler = 1
	TWAR = (addr << 1);        // direccion 7-bit
	TWCR = (1 << TWEN) | (1 << TWEA) | (1 << TWIE); // TWI + ACK + IRQ
}

// ISR: envia snapshot consistente (MSB, LSB) Y recibe comandos (rx_buf)
ISR(TWI_vect) {
	uint8_t s = TWSR & 0xF8;
	
	switch (s) {
		// ---------- SLA+W recibido: arranca una recepcion nueva ----------
		case 0x60: case 0x68:
		tx_idx = 0;
		rx_idx = 0;   // NUEVO: reinicia el indice de recepcion
		TWCR = (1 << TWINT)|(1 << TWEN)|(1 << TWEA)|(1 << TWIE);
		break;
		
		// ---------- Dato recibido (direccionado o general call) ----------
		// Antes esto se ignoraba. Ahora se guarda en rx_buf.
		case 0x80: case 0x90:
		case 0x88: case 0x98:
		if (rx_idx < 2) {
			rx_buf[rx_idx] = TWDR;
			rx_idx++;
		}
		if (rx_idx == 2) {
			cmd_ready = 1;   // ya llegaron cmd + valor completos
		}
		TWCR = (1 << TWINT)|(1 << TWEN)|(1 << TWEA)|(1 << TWIE);
		break;
		
		// ---------- Transmisor (SLA+R): logica original sin cambios ----------
		case 0xA8: // own SLA+R
		case 0xB0: // arb lost + SLA+R
		tx_idx = 0;
		TWDR = (uint8_t)((tx_snap >> 8) & 0xFF);
		tx_idx = 1;
		TWCR = (1 << TWINT)|(1 << TWEN)|(1 << TWEA)|(1 << TWIE);
		break;
		
		case 0xB8: // dato transmitido; ACK
		if (tx_idx == 1) {
			TWDR = (uint8_t)(tx_snap & 0xFF);
			tx_idx = 2;
			} else {
			TWDR = (uint8_t)(tx_snap & 0xFF);
		}
		TWCR = (1 << TWINT)|(1 << TWEN)|(1 << TWEA)|(1 << TWIE);
		break;
		
		case 0xA0: // STOP o repeated START recibido
		tx_idx = 0;
		rx_idx = 0;
		TWCR = (1 << TWINT)|(1 << TWEN)|(1 << TWEA)|(1 << TWIE);
		break;
		
		case 0xC0: // dato transmitido; NACK (fin)
		case 0xC8: // ultimo dato transmitido; ACK
		default:
		tx_idx = 0;
		TWCR = (1 << TWINT)|(1 << TWEN)|(1 << TWEA)|(1 << TWIE);
		break;
	}
}
