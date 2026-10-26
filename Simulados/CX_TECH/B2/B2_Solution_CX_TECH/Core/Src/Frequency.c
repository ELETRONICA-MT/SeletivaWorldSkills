#include "Frequency.h"

volatile uint16_t IC_FREQ = 0;
volatile uint16_t IC_FREQ_AN = 0;
volatile uint16_t IC_FREQ_AT = 0;
volatile uint8_t FIRST = 0;

extern TIM_HandleTypeDef htim2;

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
    if(htim->Instance == TIM2) {
        if(FIRST == 0) {
            IC_FREQ_AN = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
            FIRST = 1;
        } else {
            IC_FREQ_AT = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
            __HAL_TIM_SET_COUNTER(htim, 0);
            if(IC_FREQ_AT > IC_FREQ_AN)
                IC_FREQ = IC_FREQ_AT - IC_FREQ_AN;
            FIRST = 0;
        }
    }
}

float get_frequence(void){
	return 1/(IC_FREQ*0.000001);
}
