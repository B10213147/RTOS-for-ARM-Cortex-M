/*
 * rtos.c
 *
 *  Created on: July 8, 2017
 *      Author: Harvard Tseng
 */
 
/* Includes ------------------------------------------------------------------*/
#include "rtos.h"
#include "rt_HAL.h"

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
void OS_Init(uint32_t slice, enum trig_sr source){
    uint32_t slice_quantum = slice * (SystemCoreClock / 1000000);
    switch(source){
        case CM_SysTick:
            while(SysTick_Config(slice_quantum));
            break;
        case ST_Tim6:
            ST_TIM6_Config(slice_quantum);
            break;
        default:
            break;
    }
}
