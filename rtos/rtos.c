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
/* Private variables ---------------------------------------------------------*/
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
    // Initialize task TCB pointers to NULL.
    for(int i = 0; i < max_active_TCB; i++){
        os_active_TCB[i] = NULL;
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
  * @brief  Create task for RTOS.
  * @param  task_entry: Function name.
  * @param  argv: Function's arguments.
  * @retval 0 Function succeeded.
  * @retval 1 Function failed.
  */
uint8_t OS_Task_Create(voidfuncptr task_entry, void *argv){
    P_TCB task;
    task = rt_tsk_create(task_entry, argv);
    if(!task){ return 1; }  // Task create failed
    //rt_put_first(&os_rdy_tasks, task);
    rt_put_last(&os_rdy_tasks, task);

    return 0;
}

/**
  * @brief  Delete a task in RTOS.
  * @param  task: Function wait for deleted.
  * @retval 0 Function succeeded.
  * @retval 1 Function failed.
  */
uint8_t OS_Task_Delete(voidfuncptr task){
    P_TCB p_TCB;
    OS_TID tid;
    if(os_running_tsk->function == task){
        // Delete running task
        os_running_tsk->state = Inactive;
        tid = os_running_tsk->task_id;
        os_running_tsk = 0;
    }
    else{
        // Search ready list
        tid = rt_find_TID(os_rdy_tasks, task);
        if(tid != 0){
            p_TCB = os_active_TCB[tid-1];
            p_TCB->state = Inactive;
            rt_rmv_task(&os_rdy_tasks, p_TCB);
        }
    }
    
    return rt_tsk_delete(tid);
}
