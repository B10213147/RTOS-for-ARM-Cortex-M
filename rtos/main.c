#include "rtos.h"

void test1(void){
}
void test2(void){
}

int main(void){
    OS_Task_Create(test1);
    OS_Task_Create(test2);
    OS_Task_Create(__empty);
    OS_Init(1000, CM_SysTick);  // Time slice = 1ms
    while(1);
    return 0;
}
