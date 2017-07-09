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
void empty(void){
}
void foo1(void);
void foo2(void);

/* Private variables ---------------------------------------------------------*/
voidfuncptr priv_task = empty;
voidfuncptr sch_tab[] = {foo1, empty, foo2, empty};
int sch_length = sizeof(sch_tab) / sizeof(*sch_tab);

/* Private functions ---------------------------------------------------------*/
void foo1(void){
}

void foo2(void){
}
