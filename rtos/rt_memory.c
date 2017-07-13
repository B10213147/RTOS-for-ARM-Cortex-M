/*
 * rt_memory.c
 *
 *  Created on: July 13, 2017
 *      Author: Harvard Tseng
 */
 
/* Includes ------------------------------------------------------------------*/
#include "rt_memory.h"

/* Private typedef -----------------------------------------------------------*/
typedef struct mem_block{
    struct mem_block *next;
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
