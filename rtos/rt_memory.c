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

void rt_mem_insert_blk(P_MEMB *list, P_MEMB block, unsigned int size){
    P_MEMB prev, cur;
    block->size = size;
    if(*list == 0 || *list > block){
        // Empty or lowest address
        block->next = *list;
        *list = block;
    }
    else{
        // Search the list
        for(prev = *list, cur = (*list)->next; \
            cur != 0 && cur < block; \
            prev = cur, cur = cur->next);
        block->next = cur;
        prev->next = block;
    }
}

void rt_mem_remove_blk(P_MEMB *list, P_MEMB block){
    P_MEMB prev, cur;
    if(*list == block){
        // First in the list
        *list = (*list)->next;
    }
    else{
        // Sreach the list
        for(prev = *list, cur = (*list)->next; \
            cur != block && cur != 0; \
            prev = cur, cur = cur->next);
        prev->next = cur->next;
    }
    block->next = 0;
}

void rt_mem_create(P_MEM pool, char *memory, unsigned int size){
    // 4-byte alignment
    char *n_memory = (char*)(((int)memory + 0x3) & (~0x3));
    size -= n_memory - memory;  // Remove unwanted head
    size &= ~0x3U;  // Remove unwanted tail

    pool->free = 0;
    pool->used = 0;
    rt_mem_insert_blk(&(pool->free), (P_MEMB)n_memory, \
                        size - sizeof(struct mem_blk));
}
