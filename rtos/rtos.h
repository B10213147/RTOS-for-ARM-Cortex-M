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

void OSInit(uint32_t slice, triggerType source, char *memory, uint32_t size);
void OSFirstEnable(void);
void OSEnable(void);
void OSDisable(void);
uint8_t OSTaskCreate(voidfuncptr task_entry, void *argv, char *stack, uint32_t size);
uint8_t OSTaskDelete(voidfuncptr task);

extern struct mem system_memory;

#endif /* RTOS_H_ */
