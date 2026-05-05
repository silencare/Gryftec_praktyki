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
#include "cmsis_os.h"
#include "adc.h"
#include "crc.h"
#include "dcmi.h"
#include "dma2d.h"
#include "eth.h"
#include "fatfs.h"
#include "i2c.h"
#include "ltdc.h"
#include "quadspi.h"
#include "rtc.h"
#include "sai.h"
#include "sdmmc.h"
#include "spdifrx.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb_host.h"
#include "gpio.h"
#include "fmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "../../../Drivers/BSP/STM32746G-Discovery/stm32746g_discovery.h"
#include "../../../Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_lcd.h"
#include "../../../Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_ts.h"
#include <stdlib.h>
#include <math.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct{
	float x,y;
	float vx, vy;
	uint16_t radius;
	int is_cut;
	int active;
	uint32_t color;
} Fruit;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define LCD_FB_START_ADDRESS       ((uint32_t)0xC0000000)
#define SCR_BUFFER_0		((uint32_t*)LCD_FB_START_ADDRESS)
#define SCR_WIDTH  272
#define SCR_HEIGHT 480

#define MAX_FRUITS 5
#define GRAVITY 0.25f
Fruit fruits[MAX_FRUITS];

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
static void MPU_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

char bufor[32];
volatile int points = 0;

void Update_Physic(){
	for (int i = 0; i < MAX_FRUITS; i++){
		if (!fruits[i].active)
			continue;

		fruits[i].x += fruits[i].vx;
		fruits[i].y += fruits[i].vy;
		fruits[i].vy += GRAVITY;

		if (fruits[i].x - fruits[i].radius <= 0) {
			fruits[i].x = fruits[i].radius;
			fruits[i].vx = -fruits[i].vx;
		} else if (fruits[i].x + fruits[i].radius >= BSP_LCD_GetXSize()){
			fruits[i].x = BSP_LCD_GetXSize() - fruits[i].radius;
			fruits[i].vx = -fruits[i].vx;
		}

		if (fruits[i].y - fruits[i].radius < 0){
			fruits[i].y = fruits[i].radius;
			fruits[i].vy = -fruits[i].vy * 0.8f;
		}
		if (fruits[i].y > BSP_LCD_GetYSize() + 50){
			fruits[i].active = 0;
		}
	}

}

void Touch(){
	TS_StateTypeDef tsState;
	BSP_TS_GetState(&tsState);

	if(tsState.touchDetected){
		uint16_t tx = tsState.touchX[0];
		uint16_t ty = tsState.touchY[0];

		for(int i = 0; i < MAX_FRUITS; i++){
			if(fruits[i].active && !fruits[i].is_cut){
				float dx = tx - fruits[i].x;
				float dy = ty - fruits[i].y;
				if((dx*dx+dy*dy)< (fruits[i].radius * fruits[i].radius)){
					fruits[i].is_cut = 1;
					fruits[i].vy = 2;
					points++;
				}
			}
		}

	}

}


void Draw_Frame(){
	BSP_LCD_Clear(LCD_COLOR_BLACK);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_DisplayStringAt(0, 0, (uint8_t*)bufor, LEFT_MODE);
	for(int i = 0; i < MAX_FRUITS; i++){
		if (!fruits[i].active) continue;
		BSP_LCD_SetTextColor(fruits[i].color);
		if(fruits[i].is_cut){
			BSP_LCD_FillCircle(fruits[i].x - 15, fruits[i].y, fruits[i].radius*0.7f);
			BSP_LCD_FillCircle(fruits[i].x + 15, fruits[i].y, fruits[i].radius*0.7f);
		}else{
			BSP_LCD_FillCircle(fruits[i].x, fruits[i].y, fruits[i].radius);
		}
	}
}

void Spawn_Fruit(Fruit *f){
	f->x = rand() % (SCR_WIDTH - 40) + 20;
	f->y = 240;
	f->vx = (rand() % 4 - 2);
	f->vy = -(rand() % 10 + 5);
	f->radius = 20;
	f->is_cut = 0;
	f->active = 1;
	f-> color = LCD_COLOR_ORANGE;
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
//BSP_LCD_Init();
  /* USER CODE END 1 */

  /* MPU Configuration--------------------------------------------------------*/
  //MPU_Config();

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* Configure the peripherals common clocks */
  PeriphCommonClock_Config();

  /* USER CODE BEGIN SysInit */
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  //MX_ADC3_Init();
  //MX_CRC_Init();
  //MX_DCMI_Init();
  //MX_DMA2D_Init();
  //MX_ETH_Init();
  //MX_FMC_Init();
  //MX_I2C1_Init();
  //MX_I2C3_Init();
  //MX_LTDC_Init();
  //MX_QUADSPI_Init();
  //MX_RTC_Init();
  //MX_SAI2_Init();
  //MX_SDMMC1_SD_Init();
  //MX_SPDIFRX_Init();
//  MX_SPI2_Init();
//  MX_TIM1_Init();
//  MX_TIM2_Init();
//  MX_TIM3_Init();
//  MX_TIM5_Init();
//  MX_TIM8_Init();
//  MX_TIM12_Init();
//  MX_USART1_UART_Init();
//  MX_USART6_UART_Init();
//  MX_FATFS_Init();
  /* USER CODE BEGIN 2 */

  uint32_t* pxScreenBuffer = SCR_BUFFER_0;

  BSP_LCD_Init();
  BSP_TS_Init(272, 480);
  BSP_LCD_LayerDefaultInit(LTDC_ACTIVE_LAYER, (uint32_t)SCR_BUFFER_0);
  BSP_LCD_SelectLayer(LTDC_ACTIVE_LAYER);
  BSP_LCD_DisplayOn();



  BSP_LCD_FillRect(0,0,SCR_WIDTH, SCR_HEIGHT);
  BSP_LCD_DisplayStringAt(0, 240 - 65, (uint8_t *)"Ninja Fruit", CENTER_MODE);
  HAL_Delay(2000);
  BSP_LCD_FillRect(0,0,SCR_WIDTH, SCR_HEIGHT);

  TS_StateTypeDef tsState;

  for(int i=0;i<MAX_FRUITS;i++){
      fruits[i].active = 0;
  }
  //xTaskCreate(TouchScreenTaskStart, "Touch screen", 256, NULL, 6, NULL);
  //HAL_Delay(1000);
  //BSP_LCD_Clear(LCD_COLOR_BLUE);

  /* USER CODE END 2 */

  /* Call init function for freertos objects (in cmsis_os2.c) */
  //MX_FREERTOS_Init();

  /* Start scheduler */
  //osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

	  Touch();
	  Update_Physic();
	  Draw_Frame();

	  static uint32_t last_tick = 0;
	  if (HAL_GetTick() - last_tick > 1500){
		  for(int i=0; i<MAX_FRUITS; i++){
			  if(!fruits[i].active){
				  Spawn_Fruit(&fruits[i]);
				  break;
			  }
		  }
		  last_tick = HAL_GetTick();
	  }
	  sprintf(bufor, "Points: %d", points);
	  HAL_Delay(20);


    /* USER CODE BEGIN 3 */
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

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 400;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_6) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Initializes the peripherals clock
  */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC|RCC_PERIPHCLK_SAI2
                              |RCC_PERIPHCLK_SDMMC1|RCC_PERIPHCLK_CLK48;
  PeriphClkInitStruct.PLLSAI.PLLSAIN = 384;
  PeriphClkInitStruct.PLLSAI.PLLSAIR = 5;
  PeriphClkInitStruct.PLLSAI.PLLSAIQ = 2;
  PeriphClkInitStruct.PLLSAI.PLLSAIP = RCC_PLLSAIP_DIV8;
  PeriphClkInitStruct.PLLSAIDivQ = 1;
  PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_8;
  PeriphClkInitStruct.Sai2ClockSelection = RCC_SAI2CLKSOURCE_PLLSAI;
  PeriphClkInitStruct.Clk48ClockSelection = RCC_CLK48SOURCE_PLLSAIP;
  PeriphClkInitStruct.Sdmmc1ClockSelection = RCC_SDMMC1CLKSOURCE_CLK48;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

 /* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x0;
  MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
  MPU_InitStruct.SubRegionDisable = 0x87;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

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
