/*
 * I2C_Esclavo.h
 *
 * Created: 6/8/2026 12:22:12
 *  Author: crist
 */ 

#ifndef I2C_SLAVE_H
#define I2C_SLAVE_H

#include <avr/io.h>
#include <avr/interrupt.h>

// Prototipos de funciones
void I2C_Slave_Init(uint8_t addr);

// Variables globales externas (ya existian)
extern volatile uint16_t tx_snap;
extern volatile uint8_t tx_idx;

// ---- NUEVO: buffer para RECIBIR comandos del Maestro (Servo/Stepper) ----
// byte 0 = cmd   (2 = Servo, 3 = Stepper)
// byte 1 = valor (0-180 para Servo, 0-360 para Stepper)
extern volatile uint8_t rx_buf[2];
extern volatile uint8_t rx_idx;
extern volatile uint8_t cmd_ready;   // se pone en 1 cuando llegaron los 2 bytes completos

#endif
