/*
 * rt_mempool.c
 *
 *  Created on: July 17, 2017
 *      Author: Harvard Tseng
 */
 
/* Includes ------------------------------------------------------------------*/
#include "rt_TypeDef.h"
#include "rt_memory.h"
#include "rtos.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/ 
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

P_POOL rt_pool_create(uint32_t size, uint32_t blocks){
    P_POOL pool;
    OSDisable();
    
    // 4-byte alignment
    size = (size + 3U) & ~3U;
    pool = (P_POOL)rt_mem_alloc(&system_memory, 
        sizeof(struct mempool) + blocks * (size + 1));
    if(!pool){ 
        OSEnable();
        return NULL; 
    }
    pool->size = size;
    pool->blocks = blocks;
    pool->active_id = (char *)((uint32_t)pool + sizeof(struct mempool));
    pool->pool = (char *)((uint32_t)pool->active_id + blocks);
    // All blocks hasnt be used
    for(int i = 0; i < blocks; i++){
        pool->active_id[i] = 0;
    }
    
    OSEnable();
    return pool;
}

void rt_pool_distroy(P_POOL pool){
    OSDisable();
    rt_mem_free(&system_memory, pool);
    OSEnable();
}

void *rt_pool_alloc(P_POOL pool){
    int free_id;
    char *mem = NULL;
    for(free_id = 0; free_id < pool->blocks; free_id++){
        if(!pool->active_id[free_id]){ break; }
    }
    if(free_id < pool->blocks){
        pool->active_id[free_id] = 1;
        mem = pool->pool + free_id * pool->size;
    }
    return mem;
}

void rt_pool_free(P_POOL pool, void *ptr){
    int id;
    OSDisable();
    id = ((uint32_t)ptr - (uint32_t)pool->pool) / pool->size;
    if(id >= 0 && id < pool->blocks){
        pool->active_id[id] = 0;
    }
    OSEnable();
}
