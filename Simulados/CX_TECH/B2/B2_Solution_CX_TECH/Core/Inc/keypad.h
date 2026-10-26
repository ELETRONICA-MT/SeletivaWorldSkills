#ifndef KEYPANEL_H
#define KEYPANEL_H

#include "stm32l0xx_hal.h"
#include <stdint.h>
#include <stdbool.h>

#define LATCH2_Pin GPIO_PIN_0
#define LATCH2_GPIO_Port GPIOB
#define CLK_Pin GPIO_PIN_1
#define CLK_GPIO_Port GPIOB
#define DADO_Pin GPIO_PIN_8
#define DADO_GPIO_Port GPIOA
#define LINHA4_Pin GPIO_PIN_9
#define LINHA4_GPIO_Port GPIOA
#define LINHA3_Pin GPIO_PIN_3
#define LINHA3_GPIO_Port GPIOB
#define LINHA2_Pin GPIO_PIN_4
#define LINHA2_GPIO_Port GPIOB
#define LINHA1_Pin GPIO_PIN_5
#define LINHA1_GPIO_Port GPIOB

// Protótipos
void Send_74hc595(uint8_t dado);
void Matriz_teclado(bool Button[16]);

#endif
