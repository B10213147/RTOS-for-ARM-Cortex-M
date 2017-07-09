/*
 * rt_task.c
 *
 *  Created on: July 9, 2017
 *      Author: Harvard Tseng
 */
 
/* Includes ------------------------------------------------------------------*/
#include "rt_task.h"
#include "rt_TypeDef.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
int OS_Task_Create(voidfuncptr task_entry);
void __empty(void){
}

/* Private variables ---------------------------------------------------------*/
voidfuncptr priv_task = __empty;
voidfuncptr sch_tab[32];
int sch_length = 0;

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Start real time operating system.
  * @param  task_entry: Function wait for scheduled
  * @retval 0 Function succeeded.
  * @retval 1 Function failed.
  */
int OS_Task_Create(voidfuncptr task_entry){
    if(sch_length >= 32){ return 1; }
    sch_tab[sch_length] = task_entry;
    sch_length++;    
    return 0;
}
