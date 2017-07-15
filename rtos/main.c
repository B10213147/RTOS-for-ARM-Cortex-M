#include "rtos.h"

void test1(void);
void test2(void);

P_MCB Rx1;
P_MCB Tx1;
P_MCB Rx2;
P_MCB Tx2;
char memtest[500];

int main(void){
    OS_Init(1000, CM_SysTick, memtest, 500);  // Time slice = 1ms
    OS_Task_Create(test1, 0);  
    Rx1 = OS_MBX_Create(10);
    Tx1 = OS_MBX_Create(13);
    OS_Task_Create(__empty, 0);
    OS_Task_Create(test2, 0);   
    Rx2 = OS_MBX_Create(12);
    Tx2 = OS_MBX_Create(11);
    OS_Task_Create(__empty, 0);
    OS_Enable();
    
    OS_MBX_Write(Rx1, "Hello", 5);
    char a[10];
    while(1){
        OS_Disable();
        if(OS_MBX_Read(Tx1, a, 5) == 5){
            OS_MBX_Write(Rx2, a, 5);
        }
        OS_Enable();
        OS_Disable();
        if(OS_MBX_Read(Tx2, a, 5) == 5){
            OS_MBX_Write(Rx1, a, 5);
        }
        OS_Enable();
    }
    return 0;
    
}

void test1(void){
    char a[10];
    if(OS_MBX_Read(Rx1, a, 5) == 5){
        OS_MBX_Write(Tx1, a, 5);
    }
}
void test2(void){
    char a[10];
    if(OS_MBX_Read(Rx2, a, 5) == 5){
        OS_MBX_Write(Tx2, a, 5);
    }
}
