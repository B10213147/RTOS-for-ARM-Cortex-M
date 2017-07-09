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
/* Private function prototypes -----------------------------------------------*/
void empty(void){
}
void foo1(void){
}
void foo2(void){
}
void os_sched(void);

/* Private variables ---------------------------------------------------------*/
voidfuncptr priv_task = empty;
voidfuncptr sch_tab[] = {foo1, foo2};
int sch_tst = task_completed;
int sch_idx = 0;
int sch_length = sizeof(sch_tab) / sizeof(*sch_tab);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Start real time operating system.
  * @param  slice: timeslice in microseconds.
  * @retval None
  */
void OS_Init(uint32_t slice, triggerType source){
    uint32_t slice_quantum = slice * (SystemCoreClock / 1000000);
    switch(source){
        case CM_SysTick:
            while(SysTick_Config(slice_quantum));
            break;
        case ST_TIM6:
            ST_TIM6_Config(slice_quantum);
            break;
        default:
            break;
    }
}

/**
  * @brief  Entry of RTOS.
  * @param  None
  * @retval None
  */
void os_sched(void){
    if(sch_tst == task_running){ while(1); }
    sch_tst = task_running;
    
    priv_task();
    sch_tab[sch_idx]();
    
    sch_idx = (sch_idx + 1) % sch_length;
    sch_tst = task_completed;
}
