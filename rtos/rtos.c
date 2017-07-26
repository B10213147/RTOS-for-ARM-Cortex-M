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
void idle(void){
    while(1);
}

/* Private variables ---------------------------------------------------------*/
int rt_start_counter = 0;
struct mem system_memory;
P_POOL task_pool;
P_POOL list_pool;
P_POOL stack_pool;
P_POOL msgq_pool;
P_POOL mail_pool;
uint32_t slice_quantum;

/* Private functions ---------------------------------------------------------*/

/* ---------------------------------------------------------------------------*/
/*                              Kernel Control                                */
/* ---------------------------------------------------------------------------*/
  
/**
  * @brief  Start real time operating system.
  * @param  slice: Timeslice in microseconds.
  * @param  memory: Pointer to system memory.
  * @param  size: Size in byte.
  * @retval None
  */
void OSInit(uint32_t slice, char *memory, uint32_t size){
    uint32_t idle_interval;
    __set_PRIMASK(0x1U);    // CPU ignores all of interrupt requests
    slice_quantum = slice * (SystemCoreClock / 1000000);

#if (os_trigger_source == CM_SysTick)
     // Systick is a 24-bit downcount counter
    idle_interval = ((0x1U << 25) - 1) / slice_quantum;
    while(SysTick_Config(slice_quantum)); 
#elif (os_trigger_source == ST_TIM6)
    // Timer6 is a 16-bit upcount counter
    idle_interval = ((0x1U << 17) - 1) / slice_quantum;
    ST_TIM6_Config(slice_quantum);    
#endif

    // Initialize task TCB pointers to NULL.
    for(int i = 0; i < max_active_TCB; i++){
        os_active_TCB[i] = NULL;
    }
    
    rt_mem_create(&system_memory, memory, size);
    task_pool = rt_pool_create(sizeof(struct OS_TCB), max_active_TCB);
    list_pool = rt_pool_create(sizeof(struct task_list), max_active_TCB);
    stack_pool = rt_pool_create(os_stack_size + os_heap_size, max_active_TCB);
    msgq_pool = rt_pool_create(sizeof(struct msgq), max_active_TCB * 2);
    mail_pool = rt_pool_create(sizeof(struct mail_blk), max_active_TCB * 2);
    while(!task_pool || !list_pool || \
        !stack_pool || !msgq_pool || \
        !mail_pool);    // Not enough space in system_memory
    
    // Create idle task
    OSTaskCreate(idle, 0, idle_interval, 255);
}

/**
  * @brief  First time to enable RTOS.
  * @note   This function should only called in thread mode(main.c)
  * @note   and only called once.
  * @param  None
  * @retval None
  */
void OSFirstEnable(void){
    os_tsk.run = os_active_TCB[0];  // Idle task
    __set_PSP(os_tsk.run->tsk_stack + 16 * 4);
    __set_CONTROL(0x3);
    __ISB();
    OSEnable();
#if (os_trigger_source == CM_SysTick)
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;  
#elif (os_trigger_source == ST_TIM6)
    TIM6->DIER |= TIM_DIER_UIE;  
#endif 
    os_tsk.run->function();    
}

/**
  * @brief  Enable RTOS.
  * @param  None
  * @retval None
  */
void OSEnable(void){
    rt_start_counter++;
    if(rt_start_counter > 0){
        __set_PRIMASK(0x0U);
    }
}

/**
  * @brief  Disable RTOS.
  * @param  None
  * @retval None
  */
void OSDisable(void){
    __set_PRIMASK(0x1U);
    rt_start_counter--;
}

/* ---------------------------------------------------------------------------*/
/*                              Thread Control                                */
/* ---------------------------------------------------------------------------*/

/**
  * @brief  Create task for RTOS.
  * @param  task_entry: Function name.
  * @param  argv: Function's arguments.
  * @param  interval: Number of timeslices in which the task is scheduled once.
  * @param  priority: Priority of task. (Lower value means higer priority)
  * @retval 0 Function succeeded.
  * @retval 1 Function failed.
  */
uint8_t OSTaskCreate(voidfuncptr task_entry, void *argv, int interval, int priority){
    struct OS_TCB task;
    P_TCB n_task;
    
    task.function = task_entry;
    task.arg = argv;
    task.interval = interval;
    task.priority = priority;
    
    n_task = rt_tsk_create(&task);
    if(!n_task){ return 1; }  // Task create failed
    rt_put_first(&os_rdy_tasks, n_task);
    //rt_put_last(&os_rdy_tasks, n_task);

    return 0;
}

/**
  * @brief  Task can delete itself by calling this function.
  * @param  None
  * @retval None
  */
void OSTaskDeleteSelf(void){
    os_tsk.run->state = Inactive;
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
    if(os_tsk.run->function == task){
        // Delete running task
        os_tsk.run->state = Inactive;
        return 0;
    }
    
    // Search ready list
    tid = rt_find_TID(os_rdy_tasks, task);
    if(tid != 0){
        p_TCB = os_active_TCB[tid-1];
        p_TCB->state = Inactive;
        rt_rmv_task(&os_rdy_tasks, p_TCB);
    }    
    return rt_tsk_delete(tid);
}

/* ---------------------------------------------------------------------------*/
/*                              Memory Control                                */
/* ---------------------------------------------------------------------------*/

/**
  * @brief  Allocate memory space from task stack.
  * @param  size: Size in byte.
  * @retval Pointer to allocated memory.
  */
void *OSmalloc(uint32_t size){
    char *mem = NULL;
    OSDisable();
    mem = (char *)rt_mem_alloc((P_MEM)(os_tsk.run->stack), size);
    OSEnable();
    return mem;
}

/**
  * @brief  Free memory space into task stack.
  * @param  ptr: Pointer to allocated memory.
  * @retval None
  */
void OSfree(void *ptr){
    OSDisable();
    rt_mem_free((P_MEM)(os_tsk.run->stack), ptr);
    OSEnable();
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

    msg = (P_MSGQ)rt_pool_alloc(msgq_pool);
    if(!msg){ 
        OSEnable();
        return NULL; 
    }
    
    // 4-byte alignment
    size = (size + 3U) & ~3U;
    
    msg->mail = rt_mail_create(blocks * size);
    if(!msg->mail){
        rt_pool_free(msgq_pool, msg);
        OSEnable();
        return NULL;
    }
    msg->size = size;
    msg->blocks = blocks;
    
    OSEnable();
    return msg;
}

/**
  * @brief  Distroy Message Queue.
  * @param  msg: Pointer of message queue.
  * @retval None
  */
void OSMessageQDistroy(P_MSGQ msg){
    if(msg){
        OSDisable();
        rt_mail_delete(msg->mail);
        rt_pool_free(msgq_pool, msg);
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
    int i = rt_mail_write(msg->mail, data, msg->size);
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
    int i = rt_mail_read(msg->mail, data, msg->size);
    if(i == msg->size){ return 0; }
    else{ return 1; }
}
