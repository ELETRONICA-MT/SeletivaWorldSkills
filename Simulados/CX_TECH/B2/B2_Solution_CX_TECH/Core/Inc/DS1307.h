#ifndef DS1307_H
#define DS1307_H

#include "stm32l0xx_hal.h"

void DS1307_Set(uint8_t Ano, uint8_t Mes, uint8_t Dia, uint8_t Hora, uint8_t Min, uint8_t Seg);
void DS1307_Get_Array(uint8_t ArrayDados[6]);

#endif
