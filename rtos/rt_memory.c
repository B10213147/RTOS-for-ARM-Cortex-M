/*
 * rt_memory.c
 *
 *  Created on: July 13, 2017
 *      Author: Harvard Tseng
 */
 
/* Includes ------------------------------------------------------------------*/
#include "rt_memory.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/ 
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Insert a block into the list.
  * @param  list: Memory link list.
  * @param  block: Block wait for remove.
  * @param  size: Size in byte.
  * @retval None
  */
void rt_mem_insert_blk(P_MEMB *list, P_MEMB block, uint32_t size){
    P_MEMB prev, cur;
    block->size = size;
    if(!*list || *list > block){
        // Empty or lowest address
        block->next = *list;
        *list = block;
    }
    else{
        // Search the list
        for(prev = *list, cur = (*list)->next; \
            cur && cur < block; \
            prev = cur, cur = cur->next);
        block->next = cur;
        prev->next = block;
    }
}

/**
  * @brief  Remove a block from the list.
  * @param  list: Memory link list.
  * @param  block: Block wait for remove.
  * @retval None
  */
void rt_mem_remove_blk(P_MEMB *list, P_MEMB block){
    P_MEMB prev, cur;
    if(*list == block){
        // First in the list
        *list = (*list)->next;
    }
    else{
        // Sreach the list
        for(prev = *list, cur = (*list)->next; \
            cur && cur != block; \
            prev = cur, cur = cur->next);
        prev->next = cur->next;
    }
    block->next = NULL;
}

/**
  * @brief  Create a memory pool.
  * @param  pool: Pointer to memory pool.
  * @param  memory: Pointer to memory.
  * @param  size: Size in byte.
  * @retval None
  */
void rt_mem_create(P_MEM pool, char *memory, uint32_t size){
    // 4-byte alignment
    uint32_t n_memory = ((uint32_t)memory + 0x3U) & ~0x3U;
    size -= n_memory - (uint32_t)memory;  // Remove unwanted head
    size &= ~0x3U;  // Remove unwanted tail

    pool->free = NULL;
    pool->used = NULL;
    rt_mem_insert_blk(&pool->free, (P_MEMB)n_memory, \
                        size - sizeof(struct mem_blk));
}

/**
  * @brief  Allocate memory space.
  * @param  pool: Pointer to memory pool.
  * @param  size: Size in byte.
  * @retval Pointer to allocated memory.
  */
void *rt_mem_alloc(P_MEM pool, uint32_t size){
    P_MEMB cur, best = NULL;
    uint32_t delta = 0xffffffff, n_block, n_size;
    
    for(cur = pool->free; cur; cur = cur->next){
        if(cur->size >= size && \
            cur->size <= size + sizeof(struct mem_blk)){
            // Fit found
            rt_mem_remove_blk(&pool->free, cur);
            rt_mem_insert_blk(&pool->used, cur, cur->size);
            return (char *)cur + sizeof(struct mem_blk);
        }
        else if(cur->size > size && delta > cur->size - size){
            delta = cur->size - size;
            best = cur;            
        }
    }
    
    if(best){        
        n_block = (((uint32_t)best + size + sizeof(struct mem_blk) \
            + 0x3U) & ~0x3U);
        // 4-byte alignment & recalculate the size
        size = n_block - (uint32_t)best - sizeof(struct mem_blk);
        n_size = best->size - size - sizeof(struct mem_blk);
        
        rt_mem_remove_blk(&pool->free, best);
        rt_mem_insert_blk(&pool->free, (P_MEMB)n_block, n_size);
        rt_mem_insert_blk(&pool->used, best, size);    
        return (char *)best + sizeof(struct mem_blk);
    }
    
    return 0; // Not enough space
}

/**
  * @brief  Free memory space.
  * @param  pool: Pointer to memory pool.
  * @param  ptr: Pointer to allocated memory.
  * @retval None
  */
void rt_mem_free(P_MEM pool, void *ptr){
    P_MEMB cur, block = \
        (P_MEMB)((char *)ptr - sizeof(struct mem_blk));
    for(cur = pool->used; cur && cur != block; cur = cur->next);
    if(!cur){ return; } // Not found
    
    rt_mem_remove_blk(&pool->used, block);
    rt_mem_insert_blk(&pool->free, block, block->size);
    
    if((uint32_t)block + block->size + sizeof(struct mem_blk) == \
        (uint32_t)block->next){
        // Merge lower block to a continuous space
        block->size += block->next->size + sizeof(struct mem_blk);
        rt_mem_remove_blk(&pool->free, block->next);    
    }
    
    if(pool->free != block){
        for(cur = pool->free; cur->next != block; cur = cur->next);
        if((uint32_t)cur + cur->size + sizeof(struct mem_blk) == \
            (uint32_t)block){
            // Merge upper block to a continuous space
            cur->size += block->size + sizeof(struct mem_blk);
            rt_mem_remove_blk(&pool->free, block);    
        }
    }
}
