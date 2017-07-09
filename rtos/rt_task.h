/*
 * rt_task.h
 *
 *  Created on: July 9, 2017
 *      Author: Harvard Tseng
 */
 
#ifndef RT_TASK_H_
#define RT_TASK_H_

#include "rt_TypeDef.h"

int OS_Task_Create(voidfuncptr task_entry);
void __empty(void);

#endif /* RT_TASK_H_ */
