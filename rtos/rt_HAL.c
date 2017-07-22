/*
 * rt_HAL.c
 *
 *  Created on: July 8, 2017
 *      Author: Harvard Tseng
 */
 
/* Includes ------------------------------------------------------------------*/
#include "rt_HAL.h"
#include "rt_list.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void SysTick_Handler(void);
void TIM6_DAC_IRQHandler(void);
void ST_Blink(void);
void TI_Blink(void);

/* Private variables ---------------------------------------------------------*/
extern uint32_t slice_quantum;

/* Private functions ---------------------------------------------------------*/

#if os_trigger_source == CM_SysTick
/**
  * @brief  SysTick interrupt handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void){
    if(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk){
        SysTick->LOAD = slice_quantum - 0xDU;  // Calibration
        SysTick->VAL = 0;   // Any write to this register clears the SysTick counter to 0
        // Schedular
        rt_sched();
        // Sched ends

        SysTick->LOAD = SysTick->VAL + (num_of_empty - 1) * slice_quantum - 0x18U;
        SysTick->VAL = 0;   // Any write to this register clears the SysTick counter to 0
    }
}
#endif

#if os_platform == STM32F0 && os_trigger_source == ST_TIM6
/**
  * @brief  Process stack initialise.
  * @param  task: TCB of task.
  * @param  stack: Pointer of the stack memory for this task.
  * @param  size: Size of stack memory in byte.
  * @retval None
  */
void rt_init_stack(P_TCB task, char *stack, uint32_t size){
    // Stack 8-byte alignment
    uint32_t n_stack = ((uint32_t)stack + 0x3U) & ~0x3U;
    size -= n_stack - (uint32_t)stack;  // Remove unwanted head
    size &= ~0x3U;  // Remove unwanted tail
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
}

/**
  * @brief  Cortex-M context switching. (In assembly.)
  * @param  None
  * @retval None
  */
__asm void rt_context_switch(void)
{ // Context switching code
    // Simple version - assume all tasks are unprivileged
    // -------------------------
    // Save current context
    MRS     R0,     PSP     // Get current process stack pointer value
    SUBS    R0,     #32     // Allocate 32 bytes for R4 to R11
    STMIA   R0!,    {R4-R7} // Save R4 to R7 in task stack (4 regs)
    MOV     R4,     R8      // Copy R8 to R11 to R4 to R7
    MOV     R5,     R9
    MOV     R6,     R10
    MOV     R7,     R11
    STMIA   R0!,    {R4-R7} // Save R8 to R11 in task stack (4 regs)
    SUBS    R0,     #32
    LDR     R1,     =__cpp(&cur_PSP)
    LDR     R1,     [R1]    // &(os_tsk.run->tsk_stack)
    STR     R0,     [R1]    // Save PSP value into TCB(tsk_stack)
    // -------------------------
    // Load next context
    LDR     R2,     =__cpp(&os_tsk.next)
    LDR     R2,     [R2]    // Load next task from os_tsk.next
    LDR     R3,     =__cpp(&os_tsk.run)
    STR     R2,     [R3]    // Set os_tsk.run = os_tsk.next
    LDR     R4,     =__cpp(&next_PSP)
    LDR     R0,     [R4]    // Load PSP value from next_PSP
    ADDS    R0,     #16
    LDMIA   R0!,    {R4-R7} // Load R8 to R11 from task stack (4 regs)
    MOV     R8,     R4      // Copy to R8 - R11 to R4 to R7
    MOV     R9,     R5
    MOV     R10,    R6
    MOV     R11,    R7
    MSR     PSP,    R0      // Set PSP to next task
    SUBS    R0,     #32
    LDMIA   R0!,    {R4-R7} // Load R4 to R7 from task stack (4 regs)
    BX      LR              // Return
    ALIGN 4
}

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
    TIM6->CR1 |= TIM_CR1_CEN;
}

/**
  * @brief  SysTick interrupt handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void){
    if(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk){
        // Schedular
        rt_sched();
        // Sched ends
        if(os_tsk.run != os_tsk.next){
            rt_context_switch();
        }
    }
}

/**
  * @brief  TIM6 interrupt handler.
  * @param  None
  * @retval None
  */
void TIM6_DAC_IRQHandler(void){
    if(TIM6->SR & TIM_SR_UIF){
        TIM6->SR = ~TIM_SR_UIF;
        TIM6->ARR = slice_quantum - 1U;
        // Schedular
        rt_sched();
        // Sched ends
        
        TIM6->ARR = num_of_empty * slice_quantum - 1U;
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
