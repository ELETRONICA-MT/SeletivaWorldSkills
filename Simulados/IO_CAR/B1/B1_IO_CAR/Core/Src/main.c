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
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <stdio.h>
#include <stdbool.h>
#include <LCD.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

typedef struct {

	bool but[3];
	int an[3];
} in_t;

typedef struct {

	char convert[16];
} out_t;

typedef struct {

	uint32_t time, time_en;
	bool toggleservo, toggle_en;
} logic_t;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define min_servo1 40
#define min_servo2 40
#define servo1_60 105
#define servo2_60 110
#define servo1_120 165
#define servo2_120 165
#define max_servo1 235
#define max_servo2 250

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

void LCD_Write(int cmd, int dado) {

	cmd ? HAL_GPIO_WritePin(RS_GPIO_Port, RS_Pin, 1) : HAL_GPIO_WritePin(
	RS_GPIO_Port, RS_Pin, 0);

	HAL_GPIO_WritePin(D7_GPIO_Port, D7_Pin, (dado & 0x80));
	HAL_GPIO_WritePin(D6_GPIO_Port, D6_Pin, (dado & 0x40));
	HAL_GPIO_WritePin(D5_GPIO_Port, D5_Pin, (dado & 0x20));
	HAL_GPIO_WritePin(D4_GPIO_Port, D4_Pin, (dado & 0x10));

	HAL_GPIO_WritePin(EN_GPIO_Port, EN_Pin, 1);
	HAL_Delay(1);
	HAL_GPIO_WritePin(EN_GPIO_Port, EN_Pin, 0);

	HAL_GPIO_WritePin(D7_GPIO_Port, D7_Pin, (dado & 0x08));
	HAL_GPIO_WritePin(D6_GPIO_Port, D6_Pin, (dado & 0x04));
	HAL_GPIO_WritePin(D5_GPIO_Port, D5_Pin, (dado & 0x02));
	HAL_GPIO_WritePin(D4_GPIO_Port, D4_Pin, (dado & 0x01));

	HAL_GPIO_WritePin(EN_GPIO_Port, EN_Pin, 1);
	HAL_Delay(1);
	HAL_GPIO_WritePin(EN_GPIO_Port, EN_Pin, 0);
}

void LCD_String(char *ptr) {
	while (*ptr)
		LCD_Write(1, *ptr++);
}

void LCD_Start() {

	HAL_Delay(20);
	LCD_Write(0, 0x33);
	HAL_Delay(20);
	LCD_Write(0, 0x32);
	HAL_Delay(20);
	LCD_Write(0, 0x28);
	HAL_Delay(20);
	LCD_Write(0, 0x0F);
	HAL_Delay(20);
	LCD_Write(0, 0x06);
	HAL_Delay(20);
	LCD_Write(0, 0x01);
}

void task() {

	HAL_ADC_Start(&hadc);
	HAL_ADC_PollForConversion(&hadc, 10);
	in.an[0] = HAL_ADC_GetValue(&hadc);

	HAL_ADC_Start(&hadc);
	HAL_ADC_PollForConversion(&hadc, 10);
	in.an[1] = HAL_ADC_GetValue(&hadc);

	HAL_ADC_Start(&hadc);
	HAL_ADC_PollForConversion(&hadc, 10);
	in.an[2] = HAL_ADC_GetValue(&hadc);
	HAL_ADC_Stop(&hadc);

	in.but[0] = !HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin);
	in.but[1] = !HAL_GPIO_ReadPin(B2_GPIO_Port, B2_Pin);
	in.but[2] = HAL_GPIO_ReadPin(B3_GPIO_Port, B3_Pin);

	//analogs begin

	in.an[0] < 4000 ?
			HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, 1) :
			HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, 0);

	in.an[1] < 4000 ?
			HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, 1) :
			HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, 0);

	in.an[2] = in.an[2] < 60 ? 0 : in.an[2];

	in.an[2] = in.an[2] >= 4095 ? 4096 : in.an[2];

	//analogs end

	//write display begin
	sprintf(out.convert, "%d", in.but[0]);
	LCD_Write(0, 0x83);
	LCD_String(out.convert);

	sprintf(out.convert, "%d", in.but[1]);
	LCD_Write(0, 0x89);
	LCD_String(out.convert);

	sprintf(out.convert, "%d", in.but[2]);
	LCD_Write(0, 0x8F);
	LCD_String(out.convert);

	sprintf(out.convert, "%04d", in.an[2]);
	LCD_Write(0, 0xC8);
	LCD_String(out.convert);
	//write display end

	//servo begin

	if (!logic.toggleservo) {

		if (HAL_GetTick() - logic.time < 1500) {
			__HAL_TIM_SET_COMPARE(&htim22, TIM_CHANNEL_1, min_servo1);
		}

		if (HAL_GetTick() - logic.time > 1500
				&& HAL_GetTick() - logic.time < 2500) {

			__HAL_TIM_SET_COMPARE(&htim22, TIM_CHANNEL_1, servo1_120);
		}

		if (HAL_GetTick() - logic.time > 2500
				&& HAL_GetTick() - logic.time < 3300) {

			__HAL_TIM_SET_COMPARE(&htim22, TIM_CHANNEL_1, servo1_60);
		}

		if (HAL_GetTick() - logic.time > 3300
				&& HAL_GetTick() - logic.time < 4000) {

			__HAL_TIM_SET_COMPARE(&htim22, TIM_CHANNEL_1, max_servo1);
		}

		if (HAL_GetTick() - logic.time > 4000) {

			logic.time = HAL_GetTick();
			logic.toggleservo = !logic.toggleservo;
		}

	}

	if (logic.toggleservo) {

		if (HAL_GetTick() - logic.time < 1500) {
			__HAL_TIM_SET_COMPARE(&htim22, TIM_CHANNEL_2, min_servo2);
		}

		if (HAL_GetTick() - logic.time > 1500
				&& HAL_GetTick() - logic.time < 2500) {

			__HAL_TIM_SET_COMPARE(&htim22, TIM_CHANNEL_2, servo2_120);
		}

		if (HAL_GetTick() - logic.time > 2500
				&& HAL_GetTick() - logic.time < 3300) {

			__HAL_TIM_SET_COMPARE(&htim22, TIM_CHANNEL_2, servo2_60);
		}

		if (HAL_GetTick() - logic.time > 3300
				&& HAL_GetTick() - logic.time < 4000) {

			__HAL_TIM_SET_COMPARE(&htim22, TIM_CHANNEL_2, max_servo2);
		}

		if (HAL_GetTick() - logic.time > 4000) {

			logic.time = HAL_GetTick();
			logic.toggleservo = !logic.toggleservo;
		}

	}

	//servo end

}

//menu switch mode end
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
	MX_TIM22_Init();
	/* USER CODE BEGIN 2 */

	HAL_TIM_PWM_Start(&htim22, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim22, TIM_CHANNEL_2);

	LCD_Start();

	//initial menu begin
	LCD_String("    SELETIVA   ");
	LCD_Write(0, 0xC0);
	LCD_String("WORLD SKILLS  25");
	HAL_Delay(1500);
	//initial menu end

	//sub menu begin
	LCD_Write(0, 0x01);
	LCD_String("B1:   B2:   B3: ");
	LCD_Write(0, 0xC0);
	LCD_String("    POT:        ");
	LCD_Write(0, 0x0C);
	//sub menu end

	logic.time = HAL_GetTick();

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {

		task();

		//task();

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
