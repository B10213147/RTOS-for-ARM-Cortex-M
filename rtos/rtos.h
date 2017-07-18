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
#include "rt_mempool.h"

#define     max_active_TCB      32

#define     max_active_TCB      32

void OSInit(uint32_t slice, triggerType source, char *memory, uint32_t size);
void OSEnable(void);
void OSDisable(void);
uint8_t OSTaskCreate(voidfuncptr task_entry, void *argv, int interval);
uint8_t OSTaskDelete(voidfuncptr task);

extern struct mem system_memory;
extern P_POOL task_pool;
extern P_POOL list_pool;

#endif /* RTOS_H_ */
