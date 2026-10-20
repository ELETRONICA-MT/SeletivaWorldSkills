/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <lcd_panel.h>
#include <stdbool.h>
#include <stdio.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

typedef struct {

	bool but[16];
	int pot;
} in_t;

typedef struct {

	uint8_t lobat[4];
	uint8_t col[4];
} out_t;

typedef struct {

} logic_t;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

in_t in;
out_t out;
logic_t logic;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

float map(float in_x, float in_min, float in_max, float out_min, float out_max) {

	return (out_min
			+ (((in_x - in_min) / (in_max - in_min)) * (out_max - out_min)));
}

void set_freq(uint32_t freq, uint32_t duty) {

	uint32_t presc = 1000000;
	uint32_t ncountperiod = (presc / freq) - 1;
	uint32_t nduty = (ncountperiod * duty) / 100;

	__HAL_TIM_SET_AUTORELOAD(&htim21, ncountperiod);
	__HAL_TIM_SET_COMPARE(&htim21, TIM_CHANNEL_1, nduty);
}

void spi_t595(uint8_t data[4]) {

	HAL_SPI_Transmit(&hspi1, data, 4, 100);
}

void latch_c() {

	HAL_GPIO_WritePin(LATCH1_GPIO_Port, LATCH1_Pin, 0);
	HAL_GPIO_WritePin(LATCH1_GPIO_Port, LATCH1_Pin, 1);
	HAL_GPIO_WritePin(LATCH1_GPIO_Port, LATCH1_Pin, 0);
}

void set_595(uint8_t dado) {
	HAL_GPIO_WritePin(LATCH2_GPIO_Port, LATCH2_Pin, 0);
	HAL_GPIO_WritePin(CLK_GPIO_Port, CLK_Pin, 0);

	for (uint8_t i = 0; i < 8; i++) {

		HAL_GPIO_WritePin(DADO_GPIO_Port, DADO_Pin, (dado >> i) & 0x01);

		HAL_GPIO_WritePin(CLK_GPIO_Port, CLK_Pin, 1);
		HAL_GPIO_WritePin(CLK_GPIO_Port, CLK_Pin, 0);

	}
	HAL_GPIO_WritePin(LATCH2_GPIO_Port, LATCH2_Pin, 1);
	HAL_GPIO_WritePin(LATCH2_GPIO_Port, LATCH2_Pin, 0);

}

void but_matriz(bool but[16]) {

	set_595(0x80);
	in.but[0] = HAL_GPIO_ReadPin(LINHA1_GPIO_Port, LINHA1_Pin);
	in.but[1] = HAL_GPIO_ReadPin(LINHA2_GPIO_Port, LINHA2_Pin);
	in.but[2] = HAL_GPIO_ReadPin(LINHA3_GPIO_Port, LINHA3_Pin);
	in.but[3] = HAL_GPIO_ReadPin(LINHA4_GPIO_Port, LINHA4_Pin);

	set_595(0x40);
	in.but[4] = HAL_GPIO_ReadPin(LINHA1_GPIO_Port, LINHA1_Pin);
	in.but[5] = HAL_GPIO_ReadPin(LINHA2_GPIO_Port, LINHA2_Pin);
	in.but[6] = HAL_GPIO_ReadPin(LINHA3_GPIO_Port, LINHA3_Pin);
	in.but[7] = HAL_GPIO_ReadPin(LINHA4_GPIO_Port, LINHA4_Pin);

	set_595(0x20);
	in.but[8] = HAL_GPIO_ReadPin(LINHA1_GPIO_Port, LINHA1_Pin);
	in.but[9] = HAL_GPIO_ReadPin(LINHA2_GPIO_Port, LINHA2_Pin);
	in.but[10] = HAL_GPIO_ReadPin(LINHA3_GPIO_Port, LINHA3_Pin);
	in.but[11] = HAL_GPIO_ReadPin(LINHA4_GPIO_Port, LINHA4_Pin);

	set_595(0x10);
	in.but[12] = HAL_GPIO_ReadPin(LINHA1_GPIO_Port, LINHA1_Pin);
	in.but[13] = HAL_GPIO_ReadPin(LINHA2_GPIO_Port, LINHA2_Pin);
	in.but[14] = HAL_GPIO_ReadPin(LINHA3_GPIO_Port, LINHA3_Pin);
	in.but[15] = HAL_GPIO_ReadPin(LINHA4_GPIO_Port, LINHA4_Pin);
}

void task() {

	HAL_ADC_Start(&hadc);
	HAL_ADC_PollForConversion(&hadc, 10);
	in.pot = map(HAL_ADC_GetValue(&hadc), 0, 4095, 0, 1000);
	HAL_ADC_Stop(&hadc);

	if (in.pot < 19) {

		in.pot = 0;
	}

	but_matriz(in.but);

	if (in.but[12] || in.but[9]) {

		out.col[3] |= out.lobat[3];
	}

	if (in.but[6] || in.but[3]) {

		out.col[3] = 0x02;
	}

	spi_t595(out.col);

	latch_c();

	WriteInt_Display(in.pot);
}

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {

	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_ADC_Init();
	MX_I2C1_Init();
	MX_SPI1_Init();
	MX_TIM21_Init();
	/* USER CODE BEGIN 2 */

	HAL_TIM_PWM_Start(&htim21, TIM_CHANNEL_1);

	out.col[3] = 0x02;
	out.lobat[3] = 0x40;

	set_freq(261, 50);
	 HAL_Delay(1000);
	 set_freq(293, 50);
	 HAL_Delay(1000);
	 set_freq(329, 50);
	 HAL_Delay(1500);
	 set_freq(370, 50);
	 HAL_Delay(2000);
	 set_freq(415, 50);
	 HAL_Delay(1000);
	 set_freq(370, 50);
	 HAL_Delay(2000);
	 set_freq(293, 50);
	 HAL_Delay(1000);
	 set_freq(415, 50);
	 HAL_Delay(1000);
	 __HAL_TIM_SET_COMPARE(&htim21, TIM_CHANNEL_1, 0);

	spi_t595(out.col);
	latch_c();

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {

		task();

		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };
	RCC_PeriphCLKInitTypeDef PeriphClkInit = { 0 };

	/** Configure the main internal regulator output voltage
	 */
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_4;
	RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK) {
		Error_Handler();
	}
	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
	PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
	}
	/* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
