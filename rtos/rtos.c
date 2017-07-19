/*
 * rtos.c
 *
 *  Created on: July 8, 2017
 *      Author: Harvard Tseng
 */
 
/* Includes ------------------------------------------------------------------*/
#include "rtos.h"
#include "rt_HAL.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/ 
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
triggerType rt_trigger;
int rt_start_counter = 0;
struct mem system_memory;
P_POOL task_pool;
P_POOL list_pool;

/* Private functions ---------------------------------------------------------*/

/* ---------------------------------------------------------------------------*/
/*                              Kernel Control                                */
/* ---------------------------------------------------------------------------*/
  
/**
  * @brief  Start real time operating system.
  * @param  slice: timeslice in microseconds.
  * @param  source: RTOS trigger source
  *   This parameter can be one of the following values:
  *     @arg CM_SysTick
  *     @arg ST_TIM6
  * @retval None
  */
void OSInit(uint32_t slice, triggerType source, char *memory, uint32_t size){
    uint32_t slice_quantum = slice * (SystemCoreClock / 1000000);
    rt_trigger = source;
    switch(rt_trigger){
    case CM_SysTick:
        while(SysTick_Config(slice_quantum));
        SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
        break;
    case ST_TIM6:
        ST_TIM6_Config(slice_quantum);
        break;
    default:
        break;
    }
    // Initialize task TCB pointers to NULL.
    for(int i = 0; i < max_active_TCB; i++){
        os_active_TCB[i] = NULL;
    }
    
    rt_mem_create(&system_memory, memory, size);
    task_pool = rt_pool_create(sizeof(struct OS_TCB), max_active_TCB);
    list_pool = rt_pool_create(sizeof(struct task_list), max_active_TCB);
    while(!task_pool || !list_pool);    // Not enough space in system_memory
}

/**
  * @brief  Enable RTOS.
  * @param  None
  * @retval None
  */
void OSEnable(void){
    rt_start_counter++;
    if(rt_start_counter > 0){
        switch(rt_trigger){
        case CM_SysTick:
            SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
            break;
        case ST_TIM6:
            TIM6->DIER |= TIM_DIER_UIE;
            break;
        default:
            break;
        }
    }
}

/**
  * @brief  Disable RTOS.
  * @param  None
  * @retval None
  */
void OSDisable(void){
    rt_start_counter--;
    switch(rt_trigger){
    case CM_SysTick:
        SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
        break;
    case ST_TIM6:
        TIM6->DIER &= ~TIM_DIER_UIE;
        break;
    default:
        break;
    }
}

/* ---------------------------------------------------------------------------*/
/*                              Thread Control                                */
/* ---------------------------------------------------------------------------*/

/**
  * @brief  Create task for RTOS.
  * @param  task_entry: Function name.
  * @param  argv: Function's arguments.
  * @param  interval: Number of timeslices in which the task is scheduled once.
  * @retval 0 Function succeeded.
  * @retval 1 Function failed.
  */
uint8_t OSTaskCreate(voidfuncptr task_entry, void *argv, int interval){
    struct OS_TCB task;
    P_TCB n_task;
    
    task.function = task_entry;
    task.arg = argv;
    task.interval = interval;
    
    n_task = rt_tsk_create(&task);
    if(!n_task){ return 1; }  // Task create failed
    //rt_put_first(&os_rdy_tasks, task);
    rt_put_last(&os_rdy_tasks, n_task);

    return 0;
}

/**
  * @brief  Delete a task in RTOS.
  * @param  task: Function wait for deleted.
  * @retval 0 Function succeeded.
  * @retval 1 Function failed.
  */
uint8_t OSTaskDelete(voidfuncptr task){
    P_TCB p_TCB;
    OS_TID tid;
    if(os_running_tsk->function == task){
        // Delete running task
        os_running_tsk->state = Inactive;
        tid = os_running_tsk->task_id;
        os_running_tsk = 0;
    }
    else{
        // Search ready list
        tid = rt_find_TID(os_rdy_tasks, task);
        if(tid != 0){
            p_TCB = os_active_TCB[tid-1];
            p_TCB->state = Inactive;
            rt_rmv_task(&os_rdy_tasks, p_TCB);
        }
    }
    
    return rt_tsk_delete(tid);
}

/* ---------------------------------------------------------------------------*/
/*                            MessageQ Control                                */
/* ---------------------------------------------------------------------------*/

/**
  * @brief  Create Message Queue.
  * @param  size: Size of each block in byte.
  * @param  blocks: Number of block.
  * @retval Pointer of message queue.
  * @retval NULL - No message queue created.
  */
P_MSGQ OSMessageQCreate(uint32_t size, uint32_t blocks){
    P_MSGQ msg = NULL;
    OSDisable();
    
    msg = rt_mem_alloc(&system_memory, sizeof(struct msgq));
    if(!msg){ 
        OSEnable();
        return NULL; 
    }
    
    // 4-byte alignment
    size = (size + 3U) & ~3U;
    
    msg->mail = rt_mail_create(blocks * size);
    if(!msg->mail){
        rt_mem_free(&system_memory, msg);
        OSEnable();
        return NULL;
    }
    msg->size = size;
    msg->blocks = blocks;
    
    OSEnable();
    return msg;
}

/**
  * @brief  Create Message Queue.
  * @param  msg: Pointer of message queue.
  * @retval None
  */
void OSMessageQDistroy(P_MSGQ msg){
    if(msg){
        OSDisable();
        rt_mail_delete(msg->mail);
        rt_mem_free(&system_memory, msg);
        OSEnable();
    }
}

/**
  * @brief  Write a message into queue.
  * @param  msg: Pointer of message queue.
  * @param  data: A message.
  * @retval 0 Function succeeded.
  * @retval 1 Function failed.
  */
uint8_t OSMessageQWrite(P_MSGQ msg, void *data){
    OSDisable();
    int i = rt_mail_write(msg->mail, data, msg->size);
    OSEnable();
    if(i == msg->size){ return 0; }
    else{ return 1; }
}

/**
  * @brief  Read a message from queue.
  * @param  msg: Pointer of message queue.
  * @param  data: Pointer to a space where read data can put.
  * @retval 0 Function succeeded.
  * @retval 1 Function failed.
  */
uint8_t OSMessageQRead(P_MSGQ msg, void *data){
    OSDisable();    
    int i = rt_mail_read(msg->mail, data, msg->size);    
    OSEnable();
    if(i == msg->size){ return 0; }
    else{ return 1; }
}
