/*
 * rt_HAL.c
 *
 *  Created on: July 8, 2017
 *      Author: Harvard Tseng
 */
 
/* Includes ------------------------------------------------------------------*/
#include "rt_HAL.h"
#include "stm32f0xx.h"                  // Device header

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void SysTick_Handler(void);
void TIM6_DAC_IRQHandler(void);
void ST_Blink(void);
void TI_Blink(void);
extern void rt_sched(void);

/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

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
    //TIM6->DIER |= TIM_DIER_UIE;
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
        if(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk){
            // Task spent over time slice
            while(1);
        }
        else{
            return;
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
