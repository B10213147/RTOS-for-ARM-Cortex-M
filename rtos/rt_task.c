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
void __empty(void){
}

/* Private variables ---------------------------------------------------------*/
voidfuncptr priv_task = __empty;
voidfuncptr sch_tab[16];
int sch_length = 0;

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Start real time operating system.
  * @param  task_entry: Function wait for scheduled
  * @retval 0 Function succeeded.
  * @retval 1 Function failed.
  */
int OS_Task_Create(voidfuncptr task_entry){
    if(sch_length >= 16){ return 1; }
    sch_tab[sch_length] = task_entry;
    sch_length++;    
    return 0;
}

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
