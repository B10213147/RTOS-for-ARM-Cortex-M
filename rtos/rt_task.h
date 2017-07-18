/*
 * rt_task.h
 *
 *  Created on: July 9, 2017
 *      Author: Harvard Tseng
 */
 
#ifndef RT_TASK_H_
#define RT_TASK_H_

#include "rt_TypeDef.h"

OS_TID rt_get_TID(void);
P_TCB rt_tsk_create(P_TCB task);
uint8_t rt_tsk_delete(OS_TID task_id);

extern void *os_active_TCB[];

#endif /* RT_TASK_H_ */
