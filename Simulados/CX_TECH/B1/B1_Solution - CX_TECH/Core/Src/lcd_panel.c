/*
 * lcd_panel.c
 *
 *  Created on: 20 de out. de 2026
 *      Author: Aluno OC
 */

#include "lcd_panel.h"
#include "stdbool.h"

char BYTE[4];
uint32_t set_out = 0;

char DISPLAY3_BYTE0[10] = { 0x1C, // 0
		0x04, // 1
		0x2C, // 2
		0x2C, // 3
		0x34, // 4
		0x38, // 5
		0x38, // 6
		0x0C, // 7
		0x3C, // 8
		0x3C  // 9
		};

char DISPLAY3_BYTE2[10] = { 0x01, // 0
		0x01, // 1
		0x00, // 2
		0x01, // 3
		0x01, // 4
		0x01, // 5
		0x01, // 6
		0x01, // 7
		0x01, // 8
		0x01  // 9
		};
char DISPLAY3_BYTE3[10] = { 0xC0, // 0
		0x00, // 1
		0xC0, // 2
		0x80, // 3
		0x00, // 4
		0x80, // 5
		0xC0, // 6
		0x00, // 7
		0xC0, // 8
		0x00  // 9
		};

char DISPLAY2_BYTE0[10] = { 0x00, // 0
		0x00, // 1
		0x01, // 2
		0x01, // 3
		0x01, // 4
		0x01, // 5
		0x01, // 6
		0x00, // 7
		0x01, // 8
		0x01  // 9
		};
char DISPLAY2_BYTE1[10] = { 0xE0, // 0
		0x20, // 1
		0x60, // 2
		0x60, // 3
		0xA0, // 4
		0xC0, // 5
		0xC0, // 6
		0x60, // 7
		0xE0, // 8
		0xE0  // 9
		};
char DISPLAY2_BYTE2[10] = { 0x1C, // 0
		0x10, // 1
		0x0C, // 2
		0x18, // 3
		0x10, // 4
		0x18, // 5
		0x1C, // 6
		0x10, // 7
		0x1C, // 8
		0x10  // 9
		};
char DISPLAY1_BYTE1[10] = { 0x0F, // 0
		0x03, // 1
		0x16, // 2
		0x17, // 3
		0x1B, // 4
		0x1D, // 5
		0x1D, // 6
		0x07, // 7
		0x1F, // 8
		0x1F  // 9
		};
char DISPLAY1_BYTE2[10] = { 0xC0, // 0
		0x00, // 1
		0xC0, // 2
		0x80, // 3
		0x00, // 4
		0x80, // 5
		0xC0, // 6
		0x00, // 7
		0xC0, // 8
		0x00  // 9
		};

char BC[2] = { 0x00, 0x10 };

char SIMBOLOS_BYTE0[3] = { 0x00, 0x02,	// :
		0x40,	// LBT
		};
char SIMBOLOS_BYTE3[3] = { 0x00, 0x00,	// :
		0x00	// +
		};
char SINAL_BYTE0[3] = { 0x00, 0x80,	// +
		0x00	// -
		};
char SINAL_BYTE3[3] = { 0x00, 0x08,	// +
		0x08	// -
		};

char SIMBOLOS[4];

char PONTOS_BYTE2[4] = { 0x00, 0x20,	// 2DP
		0x02,	// 3DP
		0x00,	// 4DP
		};
char PONTOS_BYTE3[4] = { 0x00, 0x00,	// 2DP
		0x00,	// 3DP
		0x20,	// 4DP
		};

uint8_t COMUM = 0xF8;

// Função para inverter os bits de um byte
uint8_t invertbyte(uint8_t dado) {
	uint8_t dadoinvertido;
	bool bit[8];

	for (int i = 0; i < 8; i++) {
		bit[i] = !((dado >> i) & 0x01);
	}

	dadoinvertido = bit[7] << 7 | bit[6] << 6 | bit[5] << 5 | bit[4] << 4
			| bit[3] << 3 | bit[2] << 2 | bit[1] << 1 | bit[0];
	return dadoinvertido;
}

// Função para converter número em bytes e exibir no display via SPI
void WriteInt_Display(int16_t NUMERO) {
	uint8_t mil = 0, cent = 0, dec = 0, uni = 0;
	uint8_t BYTE_INVERT[4];

	mil = (NUMERO / 1000);
	cent = (NUMERO % 1000) / 100;
	dec = (NUMERO % 100) / 10;
	uni = (NUMERO % 10);

	BYTE[3] = (DISPLAY3_BYTE0[cent] | DISPLAY2_BYTE0[dec]);
	BYTE[2] = (DISPLAY2_BYTE1[dec] | DISPLAY1_BYTE1[uni]);
	BYTE[1] =
			(DISPLAY3_BYTE2[cent] | DISPLAY2_BYTE2[dec] | DISPLAY1_BYTE2[uni]);
	BYTE[0] = (DISPLAY3_BYTE3[cent] | BC[mil] | SIMBOLOS[1]) & COMUM;

	BYTE_INVERT[0] = invertbyte(BYTE[0] | 0x01);
	BYTE_INVERT[1] = invertbyte(BYTE[1]);
	BYTE_INVERT[2] = invertbyte(BYTE[2]);
	BYTE_INVERT[3] = invertbyte(BYTE[3]);

	extern SPI_HandleTypeDef hspi1;

	if (HAL_GetTick() - set_out > 10) {
		HAL_GPIO_WritePin(LATCH1_GPIO_Port, LATCH1_Pin, 0);

		HAL_SPI_Transmit(&hspi1, BYTE_INVERT, 4, 100);
		HAL_GPIO_WritePin(LATCH1_GPIO_Port, LATCH1_Pin, 1); // Latch
		HAL_GPIO_WritePin(LATCH1_GPIO_Port, LATCH1_Pin, 0);

	}

	if (HAL_GetTick() - set_out > 20) {
		HAL_GPIO_WritePin(LATCH1_GPIO_Port, LATCH1_Pin, 0);

		HAL_SPI_Transmit(&hspi1, BYTE, 4, 100);
		HAL_GPIO_WritePin(LATCH1_GPIO_Port, LATCH1_Pin, 1); // Latch
		HAL_GPIO_WritePin(LATCH1_GPIO_Port, LATCH1_Pin, 0);
		set_out = HAL_GetTick();
	}
}

