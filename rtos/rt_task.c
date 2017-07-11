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
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void __empty(void){
}
P_TCB rt_tsk_create(voidfuncptr task_entry, void *argv);

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
    P_TCB task;
    task = rt_tsk_create(task_entry, argv);
    /*
    if(sch_length >= MAX_TASK_N){ return 1; }
    sch_tab[sch_length] = task_entry;
    sch_length++;
    */    
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

OS_TID rt_get_TID(void){
    OS_TID i;
    for(i = 1; i <= max_active_TCB; i++){
        if(os_active_TCB[i-1] == 0){
            return i;
        }
    }
    return 0;   // os_active_TCB is full
}

P_TCB rt_tsk_create(voidfuncptr task_entry, void *argv){
    P_TCB p_task;
    OS_TID task_id;
    OS_Disable();
    
    p_task = (P_TCB)malloc(sizeof(struct OS_TCB));
    if(p_task == 0){ return 0;}   // Memory alloc failed
    p_task->function = task_entry;
    p_task->arg = argv;
    p_task->state = Ready;
    p_task->next = 0;
    
    task_id = rt_get_TID();
    if(task_id == 0){ return 0; }   // Task create failed
    os_active_TCB[task_id-1] = p_task;
    p_task->task_id = task_id;
    
    OS_Enable();
    
    return p_task;
}

int rt_tsk_delete(OS_TID task_id){
    P_TCB p_TCB;
    OS_Disable();
    
    if(task_id > max_active_TCB || os_active_TCB[task_id-1] == 0){
        return 1;   // Task not found
    }
    p_TCB = os_active_TCB[task_id-1];
    os_active_TCB[task_id-1] = 0;
    free(p_TCB);
    
    OS_Enable();
    return 0;
}
