/*
 * rt_memory.c
 *
 *  Created on: July 13, 2017
 *      Author: Harvard Tseng
 */
 
/* Includes ------------------------------------------------------------------*/
#include "rt_memory.h"

/* Private typedef -----------------------------------------------------------*/
typedef struct mem_blk{
    struct mem_blk *next;
    unsigned int size;
}*P_MEMB;

typedef struct mem{
    P_MEMB free;
    P_MEMB used;
}*P_MEM;

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
