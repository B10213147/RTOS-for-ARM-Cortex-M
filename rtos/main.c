#include "stm32f0xx.h"                  // Device header
#include "rtos.h"

int main(void){
    OS_init(10000);  // Time slice = 1ms
    while(1);
    return 0;
    /*
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
    GPIOC->MODER |= 0x1U << 2 * 9;
    while(1){
        for(int i = 0; i < 1600000; i++);
        GPIOC->ODR |= 0x1U << 9;
        for(int i = 0; i < 16000; i++);
        GPIOC->ODR &= ~(0x1U << 9);        
    }
    return 0;
    */
}

/*
#include "TM4C123.h"                    // Device header

int main(void){
    SYSCTL->RCGCGPIO |= 0x1U << 5;
    GPIOF->DIR |= 0x1U << 1;
    GPIOF->DEN |= 0x1U << 1;
    while(1){
        for(int i = 0; i < 1600000; i++);
        GPIOF->DATA |= 0x1U << 1;
        for(int i = 0; i < 16000; i++);
        GPIOF->DATA &= ~(0x1U << 1);
    }
    
    return 0;
}
*/
