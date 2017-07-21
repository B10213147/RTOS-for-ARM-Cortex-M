/*
 * rt_HAL.h
 *
 *  Created on: July 8, 2017
 *      Author: Harvard Tseng
 */
 
#ifndef RT_HAL_H_
#define RT_HAL_H_

#include <stdint.h>
#include "rt_TypeDef.h"

void rt_stack_init(P_TCB task);
void ST_TIM6_Config(uint16_t ticks);

#endif /* RT_HAL_H_ */
