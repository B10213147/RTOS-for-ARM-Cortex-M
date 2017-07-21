/*
 * rt_list.c
 *
 *  Created on: July 12, 2017
 *      Author: Harvard Tseng
 */
 
/* Includes ------------------------------------------------------------------*/
#include "rt_list.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/ 
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
struct OS_TCB *os_running_tsk = 0;
struct OS_TCB *os_rdy_tasks = 0;
int sch_tst = task_completed;
struct OS_TSK os_tsk = {0, 0};  // Running and next task info.
uint32_t cur_PSP, next_PSP;

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Insert task at the front of list.
  * @param  list: Inserting list.
  * @param  task: Task to be inserted.
  * @retval None
  */
void rt_put_first(P_TCB *list, P_TCB task){
    task->next = *list;
    *list = task;
}

/**
  * @brief  Insert task at the end of list.
  * @param  list: Inserting list.
  * @param  task: Task to be inserted.
  * @retval None
  */
void rt_put_last(P_TCB *list, P_TCB task){
    P_TCB prev = 0, cur;
    for(cur = *list; cur; prev = cur, cur = cur->next);
    if(prev){
        prev->next = task;
    }
    else{   // No task in the list
        *list = task;
    }
}

/**
  * @brief  Get first task on the list.
  * @param  list: Searching list.
  * @retval First task on the list.
  */
P_TCB rt_get_first(P_TCB *list){
    P_TCB task;
    task = *list;
    if(task){        
        *list = (*list)->next;
        task->next = NULL;
    }
    return task;
}

/**
  * @brief  Remove task from the list.
  * @param  list: Searching list.
  * @param  task: Task to be removed.
  * @retval None
  */
void rt_rmv_task(P_TCB *list, P_TCB task){
    P_TCB prev, cur;
    if(*list == task){
        // First in the list
        *list = (*list)->next;
    }
    else{
        // Sreach the list
        for(prev = *list, cur = (*list)->next; \
            cur && cur != task; \
            prev = cur, cur = cur->next);
        prev->next = cur->next;
    }
    task->next = NULL;    
}

/**
  * @brief  Find TID from the list.
  * @param  list: Searching list.
  * @param  func: Function name.
  * @retval Task id.
  * @retval 0 No task found.
  */
OS_TID rt_find_TID(P_TCB list, voidfuncptr func){
    P_TCB p_task;
    for(p_task = list; \
        p_task && p_task->function != func; \
        p_task = p_task->next);
    if(p_task != 0){ return p_task->task_id; }
    else{ return 0; }
}

/**
  * @brief  Entry of RTOS.
  * @param  None
  * @retval None
  */
void rt_sched(void){
    if(os_tsk.last){ rt_put_last(&os_rdy_tasks, os_tsk.last); }
    os_tsk.last = os_tsk.run;
    os_tsk.next = rt_get_first(&os_rdy_tasks);    
    cur_PSP = os_tsk.run->tsk_stack;
    next_PSP = os_tsk.next->tsk_stack;
    /*
    if(sch_tst == task_running){ while(1); }
    sch_tst = task_running;
    
    os_running_tsk = rt_get_first(&os_rdy_tasks);
    os_running_tsk->state = Running;
    os_running_tsk->function();
    os_running_tsk->state = Ready;
    rt_put_last(&os_rdy_tasks, os_running_tsk);
    os_running_tsk = 0;

    sch_tst = task_completed;
    */
}
