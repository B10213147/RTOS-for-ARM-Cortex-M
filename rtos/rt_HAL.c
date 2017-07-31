/*
 * rt_HAL.c
 *
 *  Created on: July 8, 2017
 *      Author: Harvard Tseng
 */
 
/* Includes ------------------------------------------------------------------*/
#include "rt_HAL.h"
#include "rt_list.h"
#include "rtos.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void PendSV_Handler(void);
void SysTick_Handler(void);
void TIM6_DAC_IRQHandler(void);
void ST_Blink(void);
void TI_Blink(void);

/* Private variables ---------------------------------------------------------*/
extern uint32_t slice_quantum;
extern int rt_start_counter;

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Process stack initialise.
  * @param  task: TCB of task.
  * @param  stack: Pointer of the stack memory for this task.
  * @param  size: Size of stack memory in byte.
  * @retval 0 Function succeeded.
  * @retval 1 Function failed.
  */
uint8_t rt_init_stack(P_TCB task, char *stack, uint32_t size){
    if(!task || !stack){ return 1; }
    // Stack 8-byte alignment
    uint32_t n_stack = ((uint32_t)stack + 0x3U) & ~0x3U;
    size -= n_stack - (uint32_t)stack;  // Remove unwanted head
    size &= ~0x3U;  // Remove unwanted tail
    if(size == 0){ return 1; }
    task->stack = (uint32_t *)n_stack;
    task->priv_stack = size;
    
    // Process Stack Pointer (PSP) value
    task->tsk_stack = (uint32_t)task->stack + task->priv_stack - 16 * 4;
    // Stack Frame format
    // -----------------------
    // 15 - xPSP
    // 14 - Return Address
    // 13 - LR
    // 12 - R12
    // 8-11 - R0 - R3
    // -------
    // 4-7 - R8 - R11
    // 0-3 - R4 - R7
    // -------
    *((uint32_t *)(task->tsk_stack + (14 << 2))) = (uint32_t)task->function;    // initial PC 
    *((uint32_t *)(task->tsk_stack + (15 << 2))) = 0x01000000;  // initial xPSR
    return 0;
}

/**
  * @brief  (Assembly)Saving Cortex-M R4-R11 registers into current task context.
  * @param  None
  * @retval None
  */
__ASM void save_R4_R11(void){
    // Save current context
    MRS     R0,     PSP     // Get current process stack pointer value
    SUBS    R0,     #32     // Allocate 32 bytes for R4 to R11
    STMIA   R0!,    {R4-R7} // Save R4 to R7 in task stack (4 regs)
    MOV     R4,     R8      // Copy R8 to R11 to R4 to R7
    MOV     R5,     R9
    MOV     R6,     R10
    MOV     R7,     R11
    STMIA   R0!,    {R4-R7} // Save R8 to R11 in task stack (4 regs)
    BX      LR              // Return 
    ALIGN 4
}

/**
  * @brief  (Assembly)Loading Cortex-M R4-R11 registers from next task context.
  * @param  None
  * @retval None
  */
__ASM void load_R4_R11(void){
    // Load next context
    MRS     R0,     PSP     // Get next process stack pointer value
    SUBS    R0,     #16
    LDMIA   R0!,    {R4-R7} // Load R8 to R11 from task stack (4 regs)
    MOV     R8,     R4      // Copy to R8 - R11 to R4 to R7
    MOV     R9,     R5
    MOV     R10,    R6
    MOV     R11,    R7
    SUBS    R0,     #32
    LDMIA   R0!,    {R4-R7} // Load R4 to R7 from task stack (4 regs)
    BX      LR              // Return    
    ALIGN 4
}

/**
  * @brief  Cortex-M context switching.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void){
    save_R4_R11();
    os_tsk.run->tsk_stack = __get_PSP() - 32;  
    __set_PSP(os_tsk.next->tsk_stack + 32);
    os_tsk.run = os_tsk.next;
    
    // Assign fresh timeslice for next task 
#if (os_trigger_source == CM_SysTick)
    SysTick->LOAD = num_of_empty * slice_quantum - 0x30U;
    SysTick->VAL = 0;   // Any write to this register clears the SysTick counter to 0
#elif (os_trigger_source == ST_TIM6)
    TIM6->CNT = 0;
#endif 

    load_R4_R11();
}

// SVC handler - Assembly wrapper to extract
// stack frame starting address
uint32_t svc_exc_return; // EXC_RETURN use by SVC
__asm void SVC_Handler(void)
{   
    MOVS    R0, #4  // Extract stack frame location
    MOV     R1, LR
    TST     R0, R1
    BEQ     stacking_used_MSP
    MRS     R0, PSP // first parameter - stacking was using PSP
    B       SVC_Handler_cont
stacking_used_MSP
    MRS     R0, MSP // first parameter - stacking was using MSP
SVC_Handler_cont
    LDR     R2,=__cpp(&svc_exc_return) // Save current EXC_RETURN
    MOV     R1, LR
    STR     R1,[R2]
    
    MOV     LR,R4
    LDMIA   R0,{R0-R3,R4}           // Read R0-R3,R12 from stack 
    MOV     R12,R4
    MOV     R4,LR
    BLX     R12                     // Call SVC Function 

    PUSH    {R4-R5}                 // Allocate two registers
    LDR     R5,=__cpp(&svc_exc_return) // Load new EXC_RETURN
    LDR     R4,[R5]
    MOVS    R3, #4
    TST     R3, R4                  // Check EXC_RETURN
    POP     {R4-R5}                 // Free two registers
    BEQ     used_MSP
    MRS     R3,PSP                  // Read PSP
    STMIA   R3!,{R0-R2}             // Store return values 
    MOVS    R0,#:NOT:0xFFFFFFFD     // Set EXC_RETURN value
    B       ready2return
used_MSP
    MRS     R3,MSP                  // Read MSP
    STMIA   R3!,{R0-R2}             // Store return values 
    MOVS    R0,#:NOT:0xFFFFFFF9     // Set EXC_RETURN value 
ready2return    
    MVNS    R0,R0
    BX      R0                      // RETI to Thread Mode

    ALIGN
}

#if os_trigger_source == CM_SysTick
/**
  * @brief  SysTick interrupt handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void){
    if(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk){
        rt_start_counter--;
        // Schedular
        rt_sched();
        // Sched ends
        if(os_tsk.run != os_tsk.next){
            SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;    // Set PendSV to pending
        }
        SysTick->LOAD = num_of_empty * slice_quantum - 0x18U;
        SysTick->VAL = 0;   // Any write to this register clears the SysTick counter to 0
        OSEnable();
    }
}
#endif

#if os_platform == STM32F0 && os_trigger_source == ST_TIM6
/**
  * @brief  Timer6 configuration.
  * @param  ticks: Number of ticks between two interrupts.
  * @retval None
  */
void ST_TIM6_Config(uint16_t ticks){
    while(!(RCC->APB1ENR & RCC_APB1ENR_TIM6EN)){
        RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
    }
    if(TIM6->CR1 & TIM_CR1_CEN){
        TIM6->CR1 &= ~TIM_CR1_CEN;
    }
    TIM6->ARR = ticks - 1U;
    TIM6->CNT = 0;
    NVIC_EnableIRQ(TIM6_DAC_IRQn);
    TIM6->DIER |= TIM_DIER_UIE;
    TIM6->CR1 |= TIM_CR1_CEN;
}

/**
  * @brief  TIM6 interrupt handler.
  * @param  None
  * @retval None
  */
void TIM6_DAC_IRQHandler(void){
    if(TIM6->SR & TIM_SR_UIF){
        TIM6->SR = ~TIM_SR_UIF;
        rt_start_counter--;
        // Schedular
        rt_sched();
        // Sched ends
        if(os_tsk.run != os_tsk.next){
            SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;    // Set PendSV to pending
        }
        TIM6->ARR = num_of_empty * slice_quantum - 1U;
        OSEnable();
    }
}
#endif

#if os_platform == STM32F0
void ST_Blink(void){
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
    GPIOC->MODER |= 0x1U << 2 * 9;
    while(1){
        for(int i = 0; i < 1600000; i++);
        GPIOC->ODR |= 0x1U << 9;
        for(int i = 0; i < 16000; i++);
        GPIOC->ODR &= ~(0x1U << 9);        
    }    
}
#endif

#if os_platform == TM4C123G
void TI_Blink(void){    
    SYSCTL->RCGCGPIO |= 0x1U << 5;
    GPIOF->DIR |= 0x1U << 1;
    GPIOF->DEN |= 0x1U << 1;
    while(1){
        for(int i = 0; i < 1600000; i++);
        GPIOF->DATA |= 0x1U << 1;
        for(int i = 0; i < 16000; i++);
        GPIOF->DATA &= ~(0x1U << 1);
    }    
}
#endif
