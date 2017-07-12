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
    P_TCB prev = os_rdy.task;
    os_rdy.task = task;
    os_rdy.next = prev;
}
