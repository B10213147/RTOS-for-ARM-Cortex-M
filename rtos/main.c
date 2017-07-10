#include "rtos.h"

void test1(void);
void test2(void);

char test1_Rx_buff[10];
OS_MCB test1_Rx = {0, 0, 10, test1_Rx_buff};
char test1_Tx_buff[10];
OS_MCB test1_Tx = {0, 0, 10, test1_Tx_buff};
char test2_Rx_buff[10];
OS_MCB test2_Rx = {0, 0, 10, test2_Rx_buff};
char test2_Tx_buff[10];
OS_MCB test2_Tx = {0, 0, 10, test2_Tx_buff};
int main(void){
    OS_Task_Create(test1);    
    OS_Task_Create(__empty);
    OS_Task_Create(test2);    
    OS_Task_Create(__empty);
    OS_Init(1000, CM_SysTick);  // Time slice = 1ms
    OS_Enable();
    
    OS_MBX_Write(&test1_Tx, "abc", 3);
    while(1){
        char tmp[5];
        OS_Disable();
        OS_MBX_Read(&test1_Tx, tmp, 3);
        OS_MBX_Write(&test2_Rx, tmp, 3);
        OS_Enable();
        OS_Disable();
        OS_MBX_Read(&test2_Tx, tmp, 3);
        OS_MBX_Write(&test1_Rx, tmp, 3);
        OS_Enable();
    }
    return 0;
}

void test1(void){
    char tmp1[5];
    OS_MBX_Read(&test1_Rx, tmp1, 3);
    OS_MBX_Write(&test1_Tx, tmp1, 3);
}
void test2(void){
    char tmp2[5];
    OS_MBX_Read(&test2_Rx, tmp2, 3);
    OS_MBX_Write(&test2_Tx, tmp2, 3);
}
