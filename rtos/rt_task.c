/*
 * rt_task.c
 *
 *  Created on: July 9, 2017
 *      Author: Harvard Tseng
 */
 
/* Includes ------------------------------------------------------------------*/
#include "rt_task.h"
#include "rtos.h"
#include <stdlib.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define     max_active_TCB      32

/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void __empty(void){
}

/* Private variables ---------------------------------------------------------*/
voidfuncptr priv_task = __empty;
voidfuncptr sch_tab[MAX_TASK_N];
int sch_length = 0;
void *os_active_TCB[max_active_TCB];

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Create task for RTOS.
  * @param  task_entry: Function wait for scheduled.
  * @retval 0 Function succeeded.
  * @retval 1 Function failed.
  */
int OS_Task_Create(voidfuncptr task_entry, void *argv){
    P_TCB p_task;
    OS_Disable();
    
    p_task = (P_TCB)malloc(sizeof(struct OS_TCB));
    if(p_task == 0){ return 1; }    // Memory alloc failed
    p_task->function = task_entry;
    p_task->arg = argv;
    p_task->state = Ready;
    p_task->next = 0;
    
    os_active_TCB[sch_length] = p_task;
    sch_length++;
    /*
    if(sch_length >= MAX_TASK_N){ return 1; }
    sch_tab[sch_length] = task_entry;
    sch_length++;
    */    
    OS_Enable();
    return 0;
}

/**
  * @brief  Delete a task in RTOS.
  * @param  task: Function wait for deleted.
  * @retval None
  */
void OS_Task_Delete(voidfuncptr task){
    for(int i = 0; i < sch_length; i++){
        if(sch_tab[i] == task){
            for(int j = i; j < sch_length; j++){
                // Shift the rest of sch_tab[]
                sch_tab[j] = sch_tab[j+1];
            }
            sch_length--;
            break;
        }
    }
}
