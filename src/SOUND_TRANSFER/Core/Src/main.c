/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd.h"
#include <string.h>
#include <stdio.h>
#include "AES.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define ARRAY_SIZE 6
#define DECODER_SIZE 80
#define THRESHOLD 500
#define DOT_DURATION_MIN 10
#define DOT_DURATION_MAX 40
#define DASH_DURATION_MIN 200
#define DASH_DURATION_MAX 350
#define TIME_OUT 300
#define X_MAX_POS 19
#define Y_MAX_POS 3
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

I2C_HandleTypeDef hi2c2;

/* USER CODE BEGIN PV */
enum MorseSymbol {
    SYMBOL_NONE,
    SYMBOL_DOT,
    SYMBOL_DASH
};
const char *codeTable[43] = {
  "01",
  "1000",
  "011",
  "110",
  "100",
  "0",
  "0001",
  "1100",
  "00",
  "0111",
  "101",
  "0100",
  "11",
  "10",
  "111",
  "0110",
  "010",
  "000",
  "1",
  "001",
  "0010",
  "0000",
  "1010",
  "1101",
  "1011",
  "1001",

  "01111",
  "00111",
  "00011",
  "00001",
  "00000",
  "10000",
  "11000",
  "11100",
  "11110",
  "11111",

  "010101",
  "101010",
  "10010",
  "100001",
  "10001",
  "110011",
  "001100",
};

//const char decodeTable[41] = "ABWGDEVZIJKLMNOPRSTUFHCQYX1234567890.,;- ";
char buffer[20] = "MOLGANOV ANDREY";
const char decodeTable[43] = {
		'A',
		'B',
		'W',
		'G',
		'D',
		'E',
		'V',
		'Z',
		'I',
		'J',
		'K',
		'L',
		'M',
		'N',
		'O',
		'P',
		'R',
		'S',
		'T',
		'U',
		'F',
		'H',
		'C',
		'Q',
		'Y',
		'X',
		'1',
		'2',
		'3',
		'4',
		'5',
		'6',
		'7',
		'8',
		'9',
		'0',
		'.',
		',',
		';',
		'-',
		' ',
		'!',
		'?',
};

uint8_t i = 0;
uint8_t data_index = 0;
uint8_t x_pos = 0;
uint8_t y_pos = 0;
uint32_t value = 0;
char str[80];
uint32_t start_time = 0;
uint32_t word_start_time = 0;
uint32_t word_time = 0;
uint32_t duration = 0;
char decoderStr[DECODER_SIZE];
uint8_t decoder_index = 0;
uint8_t count = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C2_Init(void);
static void MX_ADC1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
char decodeMorse()
{
	for (uint8_t i = 0; i < 41; i++) {
		for (uint8_t j = 0; j < data_index; j++) {
			if (buffer[j] == codeTable[i][j]) {
				count++;
				if (count == data_index) {
					data_index = 0;
					memset(buffer, 0, 20);
					return decodeTable[j];
				}
			}
		}
		count = 0;
	}
    return '?';
}

enum MorseSymbol determineMorseSymbol(uint16_t duration)
{
    if (DOT_DURATION_MIN < duration && duration < DOT_DURATION_MAX)
        return SYMBOL_DOT;
    else if (DASH_DURATION_MIN < duration && duration < DASH_DURATION_MAX)
        return SYMBOL_DASH;
    else
        return SYMBOL_NONE;
}

void Record_Data(uint16_t value)
{
	if (value < THRESHOLD && i < 15) {
		LCD_SetPos(i, 0);
		LCD_SendChar(buffer[i]);
		i++;
		LCD_SetPos(0, 1);
		sprintf(str, "%p", &buffer[i]);
		LCD_String(str);
		HAL_Delay(240);
	}
	else if (i >= 15) {
		LCD_Clear();
		i = 0;
	}

    if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1) == GPIO_PIN_SET)
    {
        if (start_time == 0)
        {
            start_time = HAL_GetTick();
        }
        else
        {
            duration = HAL_GetTick() - start_time;
            word_start_time = 0;
        }
    }
    else if (start_time != 0)
	{
    	if (word_start_time == 0)
		{
    		word_start_time = HAL_GetTick();
		}
		else
		{
			word_time = HAL_GetTick() - word_start_time;
		}

		enum MorseSymbol symbol = determineMorseSymbol(duration);

		switch (symbol)
		{
			case SYMBOL_DOT:
				buffer[data_index] = '0';
				data_index++;
				break;

			case SYMBOL_DASH:
				buffer[data_index] = '1';
				data_index++;
				break;
			default:
				break;
		}

		if (word_time > TIME_OUT) {
			decoderStr[decoder_index] = decodeMorse();
			LCD_SetPos(x_pos, y_pos);
			LCD_SendChar(decoderStr[decoder_index]);
			decoder_index++;
			x_pos++;
			if (decoder_index > DECODER_SIZE) { decoder_index = 0; }
			if (x_pos > X_MAX_POS) { x_pos = 0; y_pos++; }
			if (y_pos > Y_MAX_POS) { x_pos = 0; y_pos = 0; LCD_Clear(); }
		}

		if (data_index > ARRAY_SIZE) {
			data_index = 0;
			//LCD_Clear();
		}

		start_time = 0;
	}
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
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
  MX_I2C2_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */
  LCD_ini();
  LCD_Clear();
  //HAL_ADC_Start_IT(&hadc1);

  word_time = HAL_GetTick();
  HAL_ADC_Start_IT(&hadc1);
  value = HAL_ADC_GetValue(&hadc1);
  HAL_Delay(30);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  	HAL_ADC_Start_IT(&hadc1);
		value = HAL_ADC_GetValue(&hadc1);

		Record_Data(value);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 100;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 100000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
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
