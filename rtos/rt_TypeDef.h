/*
 * rt_TypeDef.h
 *
 *  Created on: July 8, 2017
 *      Author: Harvard Tseng
 */
 
#ifndef RT_TYPEDEF_H_
#define RT_TYPEDEF_H_

typedef void (*voidfuncptr)(void);
/*
struct OS_TCB{
    
};

struct OS_TSK{
    
};

struct OS_MCB{
    
};
*/
typedef enum{
    CM_SysTick,
    ST_TIM6
}triggerType;

typedef enum{
    task_completed,
    task_running
}sch_statusType;

#endif /* RT_TYPEDEF_H_ */
