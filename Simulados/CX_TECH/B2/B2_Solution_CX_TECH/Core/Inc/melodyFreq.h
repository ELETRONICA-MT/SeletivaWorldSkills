#ifndef MELODYFREQ_H
#define MELODYFREQ_H

#include "stm32l0xx_hal.h"

// Protótipos
void OffSpeaker();
void SetNoteRaw(float Note);
void SetNote(float Note, uint32_t DelayOn, uint32_t DelayOff);
void Speaker(float Freq);
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);
float get_frequence(void);

// Variáveis globais
extern volatile uint16_t IC_FREQ;
extern volatile uint16_t IC_FREQ_AN;
extern volatile uint16_t IC_FREQ_AT;
extern volatile uint8_t FIRST;

extern float DO[4];
extern float DO_RE[4];
extern float RE[4];extern float RE_MI[4];extern float MI[4];extern float FA[4];extern float FA_SOL[4];extern float SOL[4];extern float SOL_LA[4];extern float LA[4];extern float LA_SI[4];extern float SI[4];

#endif
