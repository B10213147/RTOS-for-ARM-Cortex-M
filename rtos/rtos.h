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
#include "rt_mailbox.h"

void OS_Init(uint32_t slice, triggerType source);

#endif /* RTOS_H_ */
