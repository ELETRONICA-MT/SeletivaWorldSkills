/*
 * lcd_panel.h
 *
 *  Created on: 20 de out. de 2026
 *      Author: Aluno OC
 */

#ifndef INC_LCD_PANEL_H_
#define INC_LCD_PANEL_H_



#endif /* INC_LCD_PANEL_H_ */


#include "stm32l0xx_hal.h" // ou outra HAL usada no seu projeto
#include <stdint.h>
#include <stdbool.h>

#define LATCH1_Pin GPIO_PIN_6
#define LATCH1_GPIO_Port GPIOA

// Protótipos das funções
void WriteInt_Display(int16_t numero);
uint8_t invertbyte(uint8_t dado);

