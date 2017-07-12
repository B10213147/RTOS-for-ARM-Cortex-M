/*
 * rt_TypeDef.h
 *
 *  Created on: July 8, 2017
 *      Author: Harvard Tseng
 */
 
#ifndef RT_TYPEDEF_H_
#define RT_TYPEDEF_H_

#include <stdint.h>

typedef     uint16_t     OS_TID;

typedef void (*voidfuncptr)(void);

typedef enum{
    Inactive,
    Ready,
    Running
}rt_stateType;

typedef struct OS_TCB{
    rt_stateType state;
    OS_TID task_id;
    struct OS_TCB *next;
	voidfuncptr function;
	void *arg;
}*P_TCB;

typedef struct OS_MCB{
    unsigned int begin, end, length;
    char *data;
}*P_MCB;

typedef enum{
    CM_SysTick,
    ST_TIM6
}triggerType;

typedef enum{
    task_completed,
    task_running
}sch_statusType;

#endif /* RT_TYPEDEF_H_ */
