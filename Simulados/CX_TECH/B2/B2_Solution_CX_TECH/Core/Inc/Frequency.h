#ifndef MELODYFREQ_H
#define MELODYFREQ_H

#include "stm32l0xx_hal.h"

// Protótipos

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);
float get_frequence(void);

// Variáveis globais
extern volatile uint16_t IC_FREQ;
extern volatile uint16_t IC_FREQ_AN;
extern volatile uint16_t IC_FREQ_AT;
extern volatile uint8_t FIRST;

#endif
