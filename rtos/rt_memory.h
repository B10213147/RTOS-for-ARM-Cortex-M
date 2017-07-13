/*
 * rt_memory.h
 *
 *  Created on: July 13, 2017
 *      Author: Harvard Tseng
 */
 
#ifndef RT_MEMORY_H_
#define RT_MEMORY_H_

#include "rt_TypeDef.h"

void rt_mem_create(P_MEM pool, char *memory, unsigned int size);
void *rt_mem_alloc(P_MEM pool, unsigned int size);

#endif /* RT_MEMORY_H_ */
