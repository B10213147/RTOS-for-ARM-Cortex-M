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
void SysTick_Handler(void);
void TIM6_DAC_IRQHandler(void);
void ST_Blink(void);
void TI_Blink(void);

/* Private variables ---------------------------------------------------------*/
extern uint32_t slice_quantum;
extern int rt_start_counter;

/* Private functions ---------------------------------------------------------*/

#if os_trigger_source == CM_SysTick
/**
  * @brief  SysTick interrupt handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void){
    if(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk){
        rt_start_counter--;
        SysTick->LOAD = slice_quantum - 0xDU;  // Calibration
        SysTick->VAL = 0;   // Any write to this register clears the SysTick counter to 0
        // Schedular
        rt_sched();
        // Sched ends

        SysTick->LOAD = SysTick->VAL + (num_of_empty - 1) * slice_quantum - 0x18U;
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
        TIM6->ARR = slice_quantum - 1U;
        // Schedular
        rt_sched();
        // Sched ends
        
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
