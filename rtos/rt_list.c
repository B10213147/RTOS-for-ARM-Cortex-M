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

/* Private functions ---------------------------------------------------------*/

void rt_put_first(P_TCB *list, P_TCB task){
    task->next = *list;
    *list = task;
}

void rt_put_last(P_TCB *list, P_TCB task){
    P_TCB prev = 0, cur;
    for(cur = *list; cur != 0; prev = cur, cur = cur->next);
    if(prev != 0){
        prev->next = task;
    }
    else{   // No task in the list
        *list = task;
    }
}

P_TCB rt_get_first(P_TCB *list){
    P_TCB task;
    task = *list;
    if(task != 0){        
        *list = (*list)->next;
        task->next = 0;
    }
    return task;
}

void rt_rmv_task(P_TCB *list, P_TCB task){
    P_TCB prev, cur;
    if(*list == task){
        // First in the list
        *list = (*list)->next;
    }
    else{
        // Sreach the list
        for(prev = *list, cur = (*list)->next; \
            cur != task; \
            prev = cur, cur = cur->next);
        prev->next = cur->next;
    }
    task->next = 0;    
}
