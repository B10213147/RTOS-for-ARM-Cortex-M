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
    // 4-byte alignment
    size = (size + 3U) & ~3U;
    pool = (P_POOL)rt_mem_alloc(&system_memory, 
        sizeof(struct mempool) + blocks * (size + 1));
    if(!pool){ return NULL; }
    pool->size = size;
    pool->blocks = blocks;
    pool->active_id = (char *)((uint32_t)pool + sizeof(struct mempool));
    pool->pool = (char *)((uint32_t)pool->active_id + blocks);
    // All blocks hasnt be used
    for(int i = 0; i < blocks; i++){
        pool->active_id[i] = 0;
    }
    return pool;
}
