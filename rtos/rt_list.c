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
struct OS_TSK os_rdy = {0, 0};

/* Private functions ---------------------------------------------------------*/

void rt_put_first(P_TCB task){
    os_rdy.task->next = os_rdy.next;
    os_rdy.next = os_rdy.task;
    os_rdy.task = task;
}

void rt_put_last(P_TCB task){
    P_TCB cur;
    if(os_rdy.next == 0){
        os_rdy.next = task;
    }
    else{
        for(cur = os_rdy.next; cur->next != 0; cur = cur->next);
        cur->next = task;
    }
}

P_TCB rt_get_first(void){
    P_TCB task, next;
    task = os_rdy.task;
    next = os_rdy.next;
    if(next != 0){
        os_rdy.next = next->next;
        next->next = 0;
    }    
    os_rdy.task = next;
    return task;
}
