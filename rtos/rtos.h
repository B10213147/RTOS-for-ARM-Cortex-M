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
#include "rt_memory.h"

void OS_Init(uint32_t slice, triggerType source);
void OS_Enable(void);
void OS_Disable(void);
uint8_t OS_Task_Create(voidfuncptr task_entry, void *argv);
uint8_t OS_Task_Delete(voidfuncptr task);

extern struct mem system_memory;

#endif /* RTOS_H_ */
