/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "math.h"
#include "adc.h"
#include "usart.h"
#include "stdio.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
	float resNTC;
/* USER CODE END Variables */
/* Definitions for TemperaturaTask */
osThreadId_t TemperaturaTaskHandle;
const osThreadAttr_t TemperaturaTask_attributes = {
  .name = "TemperaturaTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for comunicationTas */
osThreadId_t comunicationTasHandle;
const osThreadAttr_t comunicationTas_attributes = {
  .name = "comunicationTas",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
// Reverses a string 'str' of length 'len'
void reverse(char* str, int len)
{
    int i = 0, j = len - 1, temp;
    while (i < j) {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
}

// Converts a given integer x to string str[].
// d is the number of digits required in the output.
// If d is more than the number of digits in x,
// then 0s are added at the beginning.
int intToStr(int x, char str[], int d)
{
    int i = 0;
    while (x) {
        str[i++] = (x % 10) + '0';
        x = x / 10;
    }

    // If number of digits required is more, then
    // add 0s at the beginning
    while (i < d)
        str[i++] = '0';

    reverse(str, i);
    str[i] = '\0';
    return i;
}

// Converts a floating-point/double number to a string.
void ftoa(float n, char* res, int afterpoint)
{
    // Extract integer part
    int ipart = (int)n;

    // Extract floating part
    float fpart = n - (float)ipart;

    // convert integer part to string
    int i = intToStr(ipart, res, 0);

    // check for display option after point
    if (afterpoint != 0) {
        res[i] = '.'; // add dot

        // Get the value of fraction part upto given no.
        // of points after dot. The third parameter
        // is needed to handle cases like 233.007
        fpart = fpart * pow(10, afterpoint);

        intToStr((int)fpart, res + i + 1, afterpoint);
    }
}

/* USER CODE END FunctionPrototypes */

void startTemperaturaTask(void *argument);
void startComunicationTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of TemperaturaTask */
  TemperaturaTaskHandle = osThreadNew(startTemperaturaTask, NULL, &TemperaturaTask_attributes);

  /* creation of comunicationTas */
  comunicationTasHandle = osThreadNew(startComunicationTask, NULL, &comunicationTas_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_startTemperaturaTask */
/**
  * @brief  Function implementing the TemperaturaTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_startTemperaturaTask */
void startTemperaturaTask(void *argument)
{
  /* USER CODE BEGIN startTemperaturaTask */

float vol_s;


  /* Infinite loop */
  for(;;)
  {
	  HAL_ADC_Start(&hadc1);
	  if(HAL_ADC_PollForConversion(&hadc1, 1000) == HAL_OK)
	  {
		  resNTC = HAL_ADC_GetValue(&hadc1);

		  vol_s = (resNTC/4096)*3300; //presmetuvanje na naponot
		  resNTC = (vol_s * 18000)/(3300 - vol_s); //presmetuvanje na otpornost

		  resNTC = 1/(((log(resNTC/10000))/3977)+(1/298.15)) - 273.15; //presmetuvanje na temperaturata


	  }
    osDelay(1);
  }
  /* USER CODE END startTemperaturaTask */
}

/* USER CODE BEGIN Header_startComunicationTask */
/**
* @brief Function implementing the comunicationTas thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_startComunicationTask */
void startComunicationTask(void *argument)
{
  /* USER CODE BEGIN startComunicationTask */
  /* Infinite loop */
	char resNTCstring[100];



  for(;;)
  {
	  ftoa(resNTC,resNTCstring,2);
	  HAL_UART_Transmit(&huart3, resNTCstring, strlen(resNTCstring), 1000);
	  HAL_UART_Transmit(&huart3, "\r\n", 2, 1000);
    osDelay(1000);
  }
  /* USER CODE END startComunicationTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

