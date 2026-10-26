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
#include <DS1307.h>
#include <keypad.h>
#include <frequency.h>
#include <melodyFreq.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

#define max_filter 150

typedef struct {

	bool but[16];
	uint8_t date[6], hour;
	float freq;
	int digit[4];
	bool inc[4], dec[4];
	int hours, level_bat;

} in_t;

typedef struct {

	int display_value;
} out_t;

typedef struct {

	bool inc[4], dec[4], lock[3], mode, apm;
	float nota;
	int set_count;
	int humor;
	uint32_t deb, humor_time;
} logic_t;

double filter[max_filter], m_filter = 0;
uint8_t cont = 0;
float last_note;
float DO[4] = { 130.81, 261.63, 523.25, 1046.50 };
float DO_RE[4] = { 138.59, 277.18, 554.37, 1108.73 };
float RE[4] = { 146.83, 293.66, 587.33, 1174.66 };
float RE_MI[4] = { 155.56, 311.13, 622.25, 1244.51 };
float MI[4] = { 164.81, 329.63, 659.25, 1318.51 };
float FA[4] = { 174.61, 349.23, 698.46, 1396.91 };
float FA_SOL[4] = { 185.00, 369.99, 739.99, 1479.98 };
float SOL[4] = { 196.00, 392.00, 783.99, 1567.98 };
float SOL_LA[4] = { 207.65, 415.30, 830.61, 1661.22 };
float LA[4] = { 220.00, 440.00, 880.00, 1760.00, };
float LA_SI[4] = { 233.08, 466.16, 932.33, 1864.66 };
float SI[4] = { 246.94, 493.88, 987.77, 1975.53, };

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

void lobat() {

	HAL_ADC_Start(&hadc);
	HAL_ADC_PollForConversion(&hadc, 10);
	in.level_bat = HAL_ADC_GetValue(&hadc);
	HAL_ADC_Stop(&hadc);
}

void set_freq(float freq, uint32_t duty) {

	uint32_t presc = 1000000;
	float n_countperiod = (presc / freq) - 1;
	uint32_t n_duty = (n_countperiod * duty) / 100;

	__HAL_TIM_SET_AUTORELOAD(&htim21, n_countperiod);
	__HAL_TIM_SET_COMPARE(&htim21, TIM_CHANNEL_1, n_duty);

}

void config_display(bool cntrl, uint8_t digito) {

	if (cntrl) {

		if (in.inc[digito] && logic.inc[digito] == false
				&& HAL_GetTick() - logic.deb > 100) {

			in.digit[digito]++;

			logic.inc[digito] = true;
			logic.deb = HAL_GetTick();
		}

		if (in.inc[digito] == false && logic.inc[digito] == true) {

			logic.inc[digito] = false;
		}

	} else {

		if (in.dec[digito] && logic.dec[digito] == false
				&& HAL_GetTick() - logic.deb > 100) {

			in.digit[digito]--;

			logic.dec[digito] = true;
			logic.deb = HAL_GetTick();
		}

		if (in.dec[digito] == false && logic.dec[digito] == true) {

			logic.dec[digito] = false;
		}
	}

	if (in.digit[0] > 9) {

		in.digit[0] = 0;
	}

	if (in.digit[0] < 0) {

		in.digit[0] = 9;
	}

	if (in.digit[1] > 5) {

		in.digit[1] = 0;
	}

	if (in.digit[1] < 0) {

		in.digit[1] = 5;
	}

	if (in.digit[2] > 9 && in.digit[3] == 0) {

		in.digit[2] = 0;
	}

	if (in.digit[2] < 0 && in.digit[3] == 0) {

		in.digit[2] = 9;
	}

	if (in.digit[2] > 2 && in.digit[3] == 1) {

		in.digit[2] = 0;
	}

	if (in.digit[2] < 0 && in.digit[3] == 1) {

		in.digit[2] = 2;
	}

	if (in.digit[3] > 1) {

		in.digit[3] = 0;
	}

	if (in.digit[3] < 0) {

		in.digit[3] = 1;
	}

}

void apm_button() {
	if (in.but[13] && !logic.lock[1]) {

		logic.lock[1] = true;
		logic.apm = !logic.apm;
	}

	if (!in.but[13] && logic.lock[1]) {

		logic.lock[1] = false;
	}
}

void set_button() {
	if (in.but[0] && !logic.lock[0]) {

		logic.lock[0] = true;
		logic.set_count++;
		if (logic.set_count == 1) {
			in.hours = (in.hour * 100) + in.date[1];

			in.digit[0] = in.hours % 10;
			in.digit[1] = (in.hours / 10) % 10;
			in.digit[2] = (in.hours / 100) % 10;
			in.digit[3] = in.hours / 1000;

		}

	}

	if (!in.but[0] && logic.lock[0]) {

		logic.lock[0] = false;

	}
}

void back_button() {

	if (in.but[12] && !logic.lock[2]) {

		logic.lock[2] = true;
		logic.mode = false;
		logic.set_count = false;
	}

	if (!in.but[12] && logic.lock[2]) {

		logic.lock[2] = false;
	}
}

bool inRange(double var, double min, double max) {
	return var > min && var < max ? true : false;
}

void task() {

	in.freq = get_frequence();
	DS1307_Get_Array(in.date);
	Matriz_teclado(in.but);

	in.inc[0] = in.but[2];
	in.dec[0] = in.but[3];

	in.inc[1] = in.but[6];
	in.dec[1] = in.but[7];

	in.inc[2] = in.but[10];
	in.dec[2] = in.but[11];

	in.inc[3] = in.but[14];
	in.dec[3] = in.but[15];

	config_display(1, 0);
	config_display(1, 1);
	config_display(1, 2);
	config_display(1, 3);
	config_display(0, 0);
	config_display(0, 1);
	config_display(0, 2);
	config_display(0, 3);

	set_button();
	back_button();

	if (logic.set_count == 0) {

		logic.mode = false;
	}

	else if (logic.set_count == 1) {

		logic.mode = true;
	}

	else if (logic.set_count >= 2) {
		uint8_t horinha = 0, minutinho = 0;

		horinha =
				logic.apm ?
						((in.digit[3] << 4) | in.digit[2]) | 0x12 :
						(in.digit[3] << 4) | in.digit[2];
		minutinho = (in.digit[1] << 4) | in.digit[0];

		DS1307_Set(0x26, 0x10, 0x23, horinha, minutinho, 0);
		logic.set_count = 0;

	}

	if (!logic.mode) {

		in.hour = in.date[2] > 12 ? in.date[2] - 12 :
					in.date[2] < 1 ? 12 : in.date[2];

		out.display_value = (in.hour * 100) + in.date[1];

		logic.apm = in.date[2] > 12 ? 1 : 0;

	} else {

		apm_button();

		out.display_value = in.digit[0] + (in.digit[1] * 10)
				+ (in.digit[2] * 100) + (in.digit[3] * 1000);
	}

	lobat();

	logic.humor = inRange(m_filter, 50, 52) ? 1 :
					inRange(m_filter, 162, 178) ? 2 :
					inRange(m_filter, 154, 158) ? 3 : logic.humor;

	if (out.display_value == 1000 && in.level_bat >= 1095) {

		if (logic.humor == 1) {

			logic.nota = HAL_GetTick() - logic.humor_time < 700 ? DO[1] :
							HAL_GetTick() - logic.humor_time > 700 ? RE[1] : 0;

			if (HAL_GetTick() - logic.humor_time > 800) {

				__HAL_TIM_SET_COMPARE(&htim21, TIM_CHANNEL_1, 0);
				logic.humor_time = HAL_GetTick();
			}

		}

		if (logic.humor == 2) {

			logic.nota = HAL_GetTick() - logic.humor_time < 250 ? SOL[1] :
							HAL_GetTick() - logic.humor_time > 350 ? LA[1] : 0;

			if (HAL_GetTick() - logic.humor_time > 800) {

				__HAL_TIM_SET_COMPARE(&htim21, TIM_CHANNEL_1, 0);
				logic.humor_time = HAL_GetTick();
			}

		}

		if (logic.humor == 3) {

			logic.nota =
					HAL_GetTick() - logic.humor_time < 250 ? RE[1] :
					inRange(HAL_GetTick() - logic.humor_time, 250, 500) ?
							MI[1] :
					HAL_GetTick() - logic.humor_time > 500 ? RE[1] : 0;

			if (HAL_GetTick() - logic.humor_time > 800) {

				__HAL_TIM_SET_COMPARE(&htim21, TIM_CHANNEL_1, 0);
				logic.humor_time = HAL_GetTick();
			}

		}

		logic.nota != last_note ? set_freq(logic.nota, 50) : 0; // atualiza somente uma vez a cada alteração de nota

	} else {
		__HAL_TIM_SET_COMPARE(&htim21, TIM_CHANNEL_1, 0);
	}

	WriteInt_Display(out.display_value, logic.apm, in.level_bat < 1095 ? 1 : 0);

	last_note = logic.nota;
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
	MX_TIM2_Init();
	MX_TIM21_Init();
	MX_TIM22_Init();
	/* USER CODE BEGIN 2 */

	HAL_TIM_PWM_Start(&htim21, TIM_CHANNEL_1);
	HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);
	HAL_TIM_Base_Start_IT(&htim22);

	logic.humor_time = HAL_GetTick();

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

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {

	if (htim->Instance == TIM22) {

		double sum = 0;
		filter[cont] = in.freq;

		cont = cont > (max_filter - 1) ? 0 : cont + 1;

		if (cont > max_filter) {
			for (uint8_t i = 0; i < (max_filter - 1); i++) {
				sum += filter[i];
			}
			m_filter = sum / max_filter;
		}

	}

}

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
