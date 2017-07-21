/*
 * rt_TypeDef.h
 *
 *  Created on: July 8, 2017
 *      Author: Harvard Tseng
 */
 
#ifndef RT_TYPEDEF_H_
#define RT_TYPEDEF_H_

#include <stdint.h>

#define     NULL        0
typedef     uint16_t    OS_TID;

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
    
    uint16_t priv_stack;              /* Private stack size, 0= system assigned  */
    uint32_t tsk_stack;               /* Current task Stack pointer (R13)        */
    uint32_t *stack;                  /* Pointer to Task Stack memory block      */
}*P_TCB;

typedef struct OS_MCB{
    uint32_t begin, end, length;
    char *data;
}*P_MCB;

typedef struct mem_blk{
    struct mem_blk *next;
    uint32_t size;
}*P_MEMB;

typedef struct mem{
    P_MEMB free;
    P_MEMB used;
}*P_MEM;

typedef enum{
    CM_SysTick,
    ST_TIM6
}triggerType;

typedef enum{
    task_completed,
    task_running
}sch_statusType;

#endif /* RT_TYPEDEF_H_ */
