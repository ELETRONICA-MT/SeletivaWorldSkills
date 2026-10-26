
#include "stm32l0xx_hal.h" // ou outra HAL usada no seu projeto
#include <stdint.h>
#include <stdbool.h>

#define LATCH1_Pin GPIO_PIN_6
#define LATCH1_GPIO_Port GPIOA

// Protótipos das funções
void WriteInt_Display(int16_t numero, bool led, bool lobat);
uint8_t invertbyte(uint8_t dado);

