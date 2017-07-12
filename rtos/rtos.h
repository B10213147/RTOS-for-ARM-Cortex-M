/*
 * rtos.h
 *
 *  Created on: July 8, 2017
 *      Author: Harvard Tseng
 */
 
#ifndef RTOS_H_
#define RTOS_H_

#include "stm32f0xx.h"                  // Device header
#include "rt_TypeDef.h"
#include "rt_task.h"
#include "rt_list.h"
#include "rt_mailbox.h"

#define     MAX_TASK_N    16    // Maximum number of tasks in sch_tab[]

void OS_Init(uint32_t slice, triggerType source);
void OS_Enable(void);
void OS_Disable(void);



#endif /* RTOS_H_ */
