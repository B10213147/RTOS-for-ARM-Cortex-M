/*
 * rt_mempool.h
 *
 *  Created on: July 17, 2017
 *      Author: Harvard Tseng
 */
 
#ifndef RT_MEMPOOL_H_
#define RT_MEMPOOL_H_

#include "rt_TypeDef.h"

P_POOL rt_pool_create(uint32_t size, uint32_t blocks);
void rt_pool_distroy(P_POOL pool);
void *rt_pool_alloc(P_POOL pool);
void rt_pool_free(P_POOL pool, void *ptr);
    
#endif /* RT_MEMPOOL_H_ */
