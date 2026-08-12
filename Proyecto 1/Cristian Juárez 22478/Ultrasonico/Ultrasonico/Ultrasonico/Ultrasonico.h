/*
 * Ultrasonico.h
 *
 * Created: 5/8/2026 17:43:05
 *  Author: crist
 */ 
#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include <stdint.h>

/* Lectura por polling (no usa timers).
 * Devuelve cm en [2..400] o -1 si no hubo eco válido.
 */
int16_t ultrasonic_read_cm_blocking(void);

#endif