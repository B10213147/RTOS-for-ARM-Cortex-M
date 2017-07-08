/*
 * rt_HAL.c
 *
 *  Created on: July 8, 2017
 *      Author: Harvard Tseng
 */
 
/* Includes ------------------------------------------------------------------*/
#include "rt_HAL.h"
#include "stm32f0xx.h"                  // Device header

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void SysTick_Handler(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  SysTick interrupt handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void){
    if(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk){
        // Schedular
        for(int i = 0; i < 100; i++);
        static int a = 0;
        a = a % 10 + 1;
        // Sched ends
        if(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk){
            // Task spent over time slice
            while(1);
        }
        else{
            return;
        }
    }
}
