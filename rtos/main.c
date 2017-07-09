#include "rtos.h"

void test1(void);
void test2(void);

char data_char[10];
OS_MCB test1_fifo = {0, 0, 10, data_char};
char data_int[5];
OS_MCB test2_fifo = {0, 0, 5, data_int};
int main(void){
    OS_Task_Create(test1);    
    OS_Task_Create(__empty);
    OS_Task_Create(test2);    
    OS_Task_Create(__empty);
    OS_Init(1000, CM_SysTick);  // Time slice = 1ms
    while(1);
    return 0;
}

void test1(void){
    OS_MBX_Write(&test1_fifo, "123", 3);
}
void test2(void){
    OS_MBX_Write(&test2_fifo, "12", 2);
}
