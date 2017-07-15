/*
 * rt_memory.h
 *
 *  Created on: July 13, 2017
 *      Author: Harvard Tseng
 */
 
#ifndef RT_MEMORY_H_
#define RT_MEMORY_H_

#include "rt_TypeDef.h"

void rt_mem_create(P_MEM pool, char *memory, uint32_t size);
void rt_mem_delete(P_MEM pool);
void *rt_mem_alloc(P_MEM pool, uint32_t size);
void rt_mem_free(P_MEM pool, void *ptr);

#endif /* RT_MEMORY_H_ */
