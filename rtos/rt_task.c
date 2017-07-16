/*
 * rt_task.c
 *
 *  Created on: July 9, 2017
 *      Author: Harvard Tseng
 */
 
/* Includes ------------------------------------------------------------------*/
#include "rt_task.h"
#include "rtos.h"
#include "rt_list.h"
#include "rt_memory.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void __empty(void){
}

/* Private variables ---------------------------------------------------------*/
void *os_active_TCB[max_active_TCB];

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Get a none-occupied id from os_active_TCB
  * @param  None
  * @retval None-occupied id.
  * @retval 0 os_active_TCB is full.
  */
OS_TID rt_get_TID(void){
    OS_TID i;
    for(i = 1; i <= max_active_TCB; i++){
        if(os_active_TCB[i-1] == 0){
            return i;
        }
    }
    return 0;   // os_active_TCB is full
}

/**
  * @brief  Create a task control block.
  * @param  task: Pointer of waiting created task control block.
  * @retval Pointer of task control block.
  * @retval NULL - No TCB created.
  */
P_TCB rt_tsk_create(P_TCB task){
    P_TCB p_task;
    OS_TID task_id;
    OSDisable();
    
    p_task = (P_TCB)rt_mem_alloc(&system_memory, sizeof(struct OS_TCB));
    if(!p_task){ 
        // Memory alloc failed
        OSEnable();
        return NULL; 
    }   
    p_task->function = task->function;
    p_task->arg = task->arg;
    p_task->state = Ready;
    p_task->next = NULL;
    p_task->interval = task->interval;
    p_task->remain_ticks = task->interval;
    
    task_id = rt_get_TID();
    if(task_id == 0){ 
        // Task create failed
        OSEnable();
        return NULL; 
    }   
    os_active_TCB[task_id-1] = p_task;
    p_task->task_id = task_id;
    
    OSEnable();    
    return p_task;
}

/**
  * @brief  Delete a task control block.
  * @param  task_id: ID of a TCB.
  * @retval 0 Function succeeded.
  * @retval 1 Function failed.
  */
uint8_t rt_tsk_delete(OS_TID task_id){
    P_TCB p_TCB;
    OSDisable();
    
    if(task_id == 0 || \
        task_id > max_active_TCB || \
        !os_active_TCB[task_id-1]){
        // Task not found
        OSEnable();
        return 1;   
    }
    p_TCB = os_active_TCB[task_id-1];
    os_active_TCB[task_id-1] = 0;
    rt_mem_free(&system_memory, p_TCB);
    
    OSEnable();
    return 0;
}
