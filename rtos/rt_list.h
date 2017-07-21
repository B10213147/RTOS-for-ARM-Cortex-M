/*
 * rt_list.h
 *
 *  Created on: July 12, 2017
 *      Author: Harvard Tseng
 */
 
#ifndef RT_LIST_H_
#define RT_LIST_H_

#include "rt_TypeDef.h"

void rt_put_first(P_TCB *list, P_TCB task);
void rt_put_last(P_TCB *list, P_TCB task);
P_TCB rt_get_first(P_TCB *list);
void rt_rmv_task(P_TCB *list, P_TCB task);
OS_TID rt_find_TID(P_TCB list, voidfuncptr func);
void rt_sched(void);

extern struct OS_TCB *os_running_tsk;
extern struct OS_TCB *os_rdy_tasks;
extern struct OS_TSK os_tsk;

#endif /* RT_LIST_H_ */
