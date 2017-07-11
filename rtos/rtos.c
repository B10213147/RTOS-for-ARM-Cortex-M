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
void rt_sched(void);

/* Private variables ---------------------------------------------------------*/
extern voidfuncptr priv_task;
extern voidfuncptr sch_tab[];
extern int sch_length;
int sch_tst = task_completed;
int sch_idx = 0;
triggerType rt_trigger;
int rt_start_counter = 0;

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Start real time operating system.
  * @param  slice: timeslice in microseconds.
  * @param  source: RTOS trigger source
  *   This parameter can be one of the following values:
  *     @arg CM_SysTick
  *     @arg ST_TIM6
  * @retval None
  */
void OS_Init(uint32_t slice, triggerType source){
    uint32_t slice_quantum = slice * (SystemCoreClock / 1000000);
    rt_trigger = source;
    switch(rt_trigger){
    case CM_SysTick:
        while(SysTick_Config(slice_quantum));
        SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
        break;
    case ST_TIM6:
        ST_TIM6_Config(slice_quantum);
        break;
    default:
        break;
    }
}

/**
  * @brief  Enable RTOS.
  * @param  None
  * @retval None
  */
void OS_Enable(void){
    rt_start_counter++;
    if(rt_start_counter > 0){
        switch(rt_trigger){
        case CM_SysTick:
            SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
            break;
        case ST_TIM6:
            TIM6->DIER |= TIM_DIER_UIE;
            break;
        default:
            break;
        }
    }
}

/**
  * @brief  Disable RTOS.
  * @param  None
  * @retval None
  */
void OS_Disable(void){
    rt_start_counter--;
    switch(rt_trigger){
    case CM_SysTick:
        SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
        break;
    case ST_TIM6:
        TIM6->DIER &= ~TIM_DIER_UIE;
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
void rt_sched(void){
    if(sch_tst == task_running){ while(1); }
    sch_tst = task_running;
    
    priv_task();
    sch_tab[sch_idx]();
    
    sch_idx = (sch_idx + 1) % sch_length;
    sch_tst = task_completed;
}
