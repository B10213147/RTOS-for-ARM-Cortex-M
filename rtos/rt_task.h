/*
 * rt_task.h
 *
 *  Created on: July 9, 2017
 *      Author: Harvard Tseng
 */
 
#ifndef RT_TASK_H_
#define RT_TASK_H_

#include "rt_TypeDef.h"

#define     max_active_TCB      32

int OS_Task_Create(voidfuncptr task_entry, void *argv);
//void OS_Task_Delete(voidfuncptr task);
void __empty(void);

extern void *os_active_TCB[];

#endif /* RT_TASK_H_ */
