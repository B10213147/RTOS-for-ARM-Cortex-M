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
    int interval;
    int remain_ticks;
}*P_TCB;

typedef struct task_list{
    struct OS_TCB *task;
    struct task_list *next;
}*P_LIST;

typedef struct mail_blk{
    uint32_t begin, end, length;
    char *data;
}*P_MAIL;

typedef struct mem_blk{
    struct mem_blk *next;
    uint32_t size;
}*P_MEMB;

typedef struct mem{
    P_MEMB free;
    P_MEMB used;
}*P_MEM;

typedef struct mempool{
    char *active_id;
    char *pool;
    uint32_t size;
    uint32_t blocks;
}*P_POOL;

typedef enum{
    CM_SysTick,
    ST_TIM6
}triggerType;

typedef enum{
    task_completed,
    task_running
}sch_statusType;

#endif /* RT_TYPEDEF_H_ */
