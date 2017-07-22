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

#define     max_active_TCB      8

void OSInit(uint32_t slice, triggerType source, char *memory, uint32_t size);
void OSFirstEnable(void);
void OSEnable(void);
void OSDisable(void);
uint8_t OSTaskCreate(voidfuncptr task_entry, void *argv, int interval, char *stack, uint32_t size);
uint8_t OSTaskDelete(voidfuncptr task);
P_MSGQ OSMessageQCreate(uint32_t size, uint32_t blocks);
void OSMessageQDistroy(P_MSGQ msg);
uint8_t OSMessageQWrite(P_MSGQ msg, void *data);
uint8_t OSMessageQRead(P_MSGQ msg, void *data);

extern struct mem system_memory;
extern P_POOL task_pool;
extern P_POOL list_pool;

#endif /* RTOS_H_ */
