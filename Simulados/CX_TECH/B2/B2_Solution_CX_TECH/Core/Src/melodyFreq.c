#include "melodyFreq.h"

/*volatile uint16_t IC_FREQ = 0;
volatile uint16_t IC_FREQ_AN = 0;
volatile uint16_t IC_FREQ_AT = 0;
volatile uint8_t FIRST = 0;

float DO[4]={130.81, 261.63, 523.25, 1046.50};
float DO_RE[4]={138.59,	277.18,	554.37,	1108.73};
float RE[4]={146.83, 293.66, 587.33, 1174.66};
float RE_MI[4]={155.56,	311.13,	622.25,	1244.51};
float MI[4]={164.81, 329.63, 659.25, 1318.51};
float FA[4]={174.61, 349.23, 698.46, 1396.91};
float FA_SOL[4]={185.00, 369.99, 739.99, 1479.98};
float SOL[4]={196.00, 392.00, 783.99, 1567.98};
float SOL_LA[4]={207.65, 415.30, 830.61, 1661.22};
float LA[4]={220.00, 440.00, 880.00, 1760.00,};
float LA_SI[4]={233.08,	466.16,	932.33,	1864.66};
float SI[4]={246.94, 493.88, 987.77, 1975.53,};

extern TIM_HandleTypeDef htim21;
extern TIM_HandleTypeDef htim2;

void Speaker(float Freq) {
    uint16_t Presc = 0, Period = 100;
    Presc = 32000000 / Freq / Period;

    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};

    htim21.Instance = TIM21;
    htim21.Init.Prescaler = Presc - 1;
    htim21.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim21.Init.Period = Period - 1;
    htim21.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim21.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    if (HAL_TIM_Base_Init(&htim21) != HAL_OK) Error_Handler();

    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim21, &sClockSourceConfig) != HAL_OK) Error_Handler();

    if (HAL_TIM_PWM_Init(&htim21) != HAL_OK) Error_Handler();

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim21, &sMasterConfig) != HAL_OK) Error_Handler();

    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;

    if (HAL_TIM_PWM_ConfigChannel(&htim21, &sConfigOC, TIM_CHANNEL_1) != HAL_OK) Error_Handler();
    if (HAL_TIM_PWM_ConfigChannel(&htim21, &sConfigOC, TIM_CHANNEL_2) != HAL_OK) Error_Handler();

    HAL_TIM_MspPostInit(&htim21);
}


void OffSpeaker(){
    __HAL_TIM_SET_COMPARE(&htim21, TIM_CHANNEL_1, 0);
}
void SetNoteRaw(float Note) {
    Speaker(Note);
    HAL_TIM_PWM_Start(&htim21, TIM_CHANNEL_1);
    __HAL_TIM_SET_COMPARE(&htim21, TIM_CHANNEL_1, 50);
}

void SetNote(float Note, uint32_t DelayOn, uint32_t DelayOff) {
	SetNoteRaw(Note);
    HAL_Delay(DelayOn);
    OffSpeaker();
    HAL_Delay(DelayOff);
}

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
*/
