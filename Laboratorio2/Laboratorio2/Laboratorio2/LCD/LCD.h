/*
 * LCD.h
 *
 * Created: 16/7/2026 13:26:42
 *  Author: crist
 */ 


#ifndef LCD_H_
#define LCD_H_
#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>



void initLCD8bits(void);

void LCD_Port(char a);

void LCD_CMD(char a);

void LCD_Write_Char(char c);

//void LCD_Write_String(char *c);

void LCD_Write_String(char *str);

void LCD_Shift_Right(void);

void LCD_Shift_Left(void);

void LCD_Set_Cursor(char f, char c);

void LCD_clear(void);

#endif /* LCD_H_ */