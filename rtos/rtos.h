/*
 * rtos.h
 *
 *  Created on: July 8, 2017
 *      Author: Harvard Tseng
 */
 
#ifndef RTOS_H_
#define RTOS_H_

#include "rt_TypeDef.h"
#include "rt_task.h"
#include "rt_list.h"
#include "rt_mailbox.h"
#include "rt_memory.h"
#include "rt_mempool.h"

// Platform
#define     STM32F0             1U
#define     TM4C123G            2U

#define     os_platform         STM32F0

// Trigger source
#define     CM_SysTick          1U
#define     ST_TIM6             2U

#define     os_trigger_source   CM_SysTick

// Maximum number of tasks to manage
#define     max_active_TCB      16

// Kernel Control
void OSInit(uint32_t slice, char *memory, uint32_t size);
void OSEnable(void);
void OSDisable(void);
// Thread Control 
uint8_t OSTaskCreate(voidfuncptr task_entry, void *argv, int interval, int priority);
void OSTaskDeleteSelf(void);
uint8_t OSTaskDelete(voidfuncptr task);
// Memory Control 
void *OSmalloc(uint32_t size);
void OSfree(void *ptr);
// MessageQ Control 
P_MSGQ OSMessageQCreate(uint32_t size, uint32_t blocks);
void OSMessageQDistroy(P_MSGQ msg);
uint8_t OSMessageQWrite(P_MSGQ msg, void *data);
uint8_t OSMessageQRead(P_MSGQ msg, void *data);

extern struct mem system_memory;
extern P_POOL task_pool;
extern P_POOL list_pool;

#endif /* RTOS_H_ */
