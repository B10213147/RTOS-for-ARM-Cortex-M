/*
 * rt_list.c
 *
 *  Created on: July 12, 2017
 *      Author: Harvard Tseng
 */
 
/* Includes ------------------------------------------------------------------*/
#include "rt_list.h"
#include "rtos.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/ 
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
struct OS_TCB *os_running_tsk = 0;
struct OS_TCB *os_rdy_tasks = 0;
int sch_tst = task_completed;

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
  * @brief  Updated a whole list ready to be excecuted.
  * @param  None
  * @retval Scheduled list.
  * @retval 0 No list created.
  */
P_LIST rt_list_updated(void){
    P_TCB task;
    P_LIST another, prev, cur, list = NULL;
    for(task = os_rdy_tasks; task; task = task->next){
        task->remain_ticks--;
        if(task->remain_ticks > 0){

        }
        else{
            // Another ready to be scheduled task
            another = (P_LIST)rt_pool_alloc(list_pool);
            another->task = task;
            another->next = NULL;
            
            for(prev = 0, cur = list; cur; prev = cur, cur = cur->next){
                if(task->priority < cur->task->priority || \
                    (task->priority == cur->task->priority &&
                    task->remain_ticks < cur->task->remain_ticks)){
                    if(prev){ prev->next = another; }
                    else{ list = another; }
                    another->next = cur;
                    break;
                }
            }

            if(!cur){
                if(prev){
                    // Reach the last of the list
                    prev->next = another;
                }
                else{
                    // List is empty
                    list = another;
                }
            } 
        }
    }
    return list;
}

/**
  * @brief  Remove first item in the scheduled list.
  * @param  list: Scheduled list.
  * @retval Task.
  * @retval 0 No task has been scheduled.
  */
P_TCB rt_rmv_list(P_LIST *list){
    P_TCB task = NULL;
    if(*list){
        task = (*list)->task;
        rt_pool_free(list_pool, *list);
        *list = (*list)->next;
    }
    return task;
}

/**
  * @brief  Entry of RTOS.
  * @param  None
  * @retval None
  */
void rt_sched(void){
    static P_LIST list = NULL;
    if(sch_tst == task_running){ while(1); }
    while(list){ rt_rmv_list(&list); }
    list = rt_list_updated();
    if(list){
        sch_tst = task_running;
        
        os_running_tsk = rt_rmv_list(&list);
        os_running_tsk->state = Running;
        os_running_tsk->function();
        os_running_tsk->state = Ready;
        os_running_tsk->remain_ticks += os_running_tsk->interval;
        os_running_tsk = 0;
    }
    sch_tst = task_completed;
}
