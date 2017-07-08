/*
 * rtos.c
 *
 *  Created on: July 8, 2017
 *      Author: Harvard Tseng
 */
 
/* Includes ------------------------------------------------------------------*/
#include "rtos.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/ 
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Start real time operating system.
  * @param  slice: timeslice in microseconds.
  * @retval None
  */
void OS_init(uint32_t slice){
    uint32_t slice_quantum = slice * (SystemCoreClock / 1000000);
    while(SysTick_Config(slice_quantum));
}
