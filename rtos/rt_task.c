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
#include <stdlib.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void __empty(void){
}
P_TCB rt_tsk_create(voidfuncptr task_entry, void *argv);
int rt_tsk_delete(OS_TID task_id);

/* Private variables ---------------------------------------------------------*/
void *os_active_TCB[max_active_TCB];

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Create task for RTOS.
  * @param  task_entry: Function name.
  * @param  argv: Function's arguments.
  * @retval 0 Function succeeded.
  * @retval 1 Function failed.
  */
int OS_Task_Create(voidfuncptr task_entry, void *argv){
    P_TCB task;
    task = rt_tsk_create(task_entry, argv);
    if(task == 0){ return 1; }  // Task create failed
    //rt_put_first(&os_rdy_tasks, task);
    rt_put_last(&os_rdy_tasks, task);

    return 0;
}

/**
  * @brief  Delete a task in RTOS.
  * @param  task: Function wait for deleted.
  * @retval None
  */
void OS_Task_Delete(voidfuncptr task){
    P_TCB p_TCB;
    if(os_running_tsk->function == task){
        // Delete running task
        p_TCB = os_running_tsk;
        os_running_tsk = 0;
    }
    else{
        // Search ready list
        
    }
    /*
    for(int i = 0; i < sch_length; i++){
        if(os_ready_tasks[i]->function == task){   
            p_TCB = os_ready_tasks[i];
            for(int j = i; j < sch_length; j++){
                // Shift the rest of os_ready_tasks[]
                os_ready_tasks[j] = os_ready_tasks[j+1];
            }
            sch_length--;
            rt_tsk_delete(p_TCB->task_id);
            break;
        }
    }
    */
}

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
  * @param  task_entry: Function name.
  * @param  argv: Function's arguments.
  * @retval Pointer of task control block.
  * @retval 0 No TCB created.
  */
P_TCB rt_tsk_create(voidfuncptr task_entry, void *argv){
    P_TCB p_task;
    OS_TID task_id;
    OS_Disable();
    
    p_task = (P_TCB)malloc(sizeof(struct OS_TCB));
    if(p_task == 0){ 
        // Memory alloc failed
        OS_Enable();
        return 0; 
    }   
    p_task->function = task_entry;
    p_task->arg = argv;
    p_task->state = Ready;
    p_task->next = 0;
    
    task_id = rt_get_TID();
    if(task_id == 0){ 
        // Task create failed
        OS_Enable();
        return 0; 
    }   
    os_active_TCB[task_id-1] = p_task;
    p_task->task_id = task_id;
    
    OS_Enable();    
    return p_task;
}

/**
  * @brief  Delete a task control block.
  * @param  task_id: ID of a TCB.
  * @retval 0 Function succeeded.
  * @retval 1 Function failed.
  */
int rt_tsk_delete(OS_TID task_id){
    P_TCB p_TCB;
    OS_Disable();
    
    if(task_id > max_active_TCB || os_active_TCB[task_id-1] == 0){
        // Task not found
        OS_Enable();
        return 1;   
    }
    p_TCB = os_active_TCB[task_id-1];
    os_active_TCB[task_id-1] = 0;
    free(p_TCB);
    
    OS_Enable();
    return 0;
}
