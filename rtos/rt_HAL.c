/*
 * rt_HAL.c
 *
 *  Created on: July 8, 2017
 *      Author: Harvard Tseng
 */
 
/* Includes ------------------------------------------------------------------*/
#include "rt_HAL.h"
#include "rt_list.h"
#include "stm32f0xx.h"                  // Device header

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void SysTick_Handler(void);
void TIM6_DAC_IRQHandler(void);
void ST_Blink(void);
void TI_Blink(void);

/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

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
            // rt_context_switch
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
        // Schedular
        rt_sched();
        // Sched ends
        if(TIM6->SR & TIM_SR_UIF){
            // Task spent over time slice
            while(1);
        }
        else{
            return;
        }
    }
}

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

void TI_Blink(void){
    /*
    SYSCTL->RCGCGPIO |= 0x1U << 5;
    GPIOF->DIR |= 0x1U << 1;
    GPIOF->DEN |= 0x1U << 1;
    while(1){
        for(int i = 0; i < 1600000; i++);
        GPIOF->DATA |= 0x1U << 1;
        for(int i = 0; i < 16000; i++);
        GPIOF->DATA &= ~(0x1U << 1);
    }
    */
}
