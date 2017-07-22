#include "rtos.h"

void test1(void);
void test2(void);

P_MSGQ Rx1;
P_MSGQ Tx1;
P_MSGQ Rx2;
P_MSGQ Tx2;
char memtest[5000];
int task1_stack[32];
int task2_stack[32];

int main(void){
    OSInit(100, memtest, 5000);  // Time slice = 1ms
    OSTaskCreate(test1, 0, 2, 1, (char *)task1_stack, sizeof(task1_stack));  
    Rx1 = OSMessageQCreate(5, 5);
    Tx1 = OSMessageQCreate(6, 5);
    OSTaskCreate(test2, 0, 10, 0, (char *)task2_stack, sizeof(task2_stack));   
    Rx2 = OSMessageQCreate(7, 5);
    Tx2 = OSMessageQCreate(8, 5);
    OSMessageQWrite(Rx1, "Hello");
    OSFirstEnable();
    
    // Should not be here
    
    
    char a[5];
    while(1){
        if(!OSMessageQRead(Tx1, a)){
            OSMessageQWrite(Rx2, a);
        }
        if(!OSMessageQRead(Tx2, a)){
            OSMessageQWrite(Rx1, a);
        }
    }
    return 0;
    
}

void test1(void){
    char a[10];
    while(1){
        OSDisable();
        if(!OSMessageQRead(Rx1, a)){
            OSMessageQWrite(Tx1, a);
        }
        OSEnable();
    }
}
void test2(void){
    char a[10];
    while(1){
        OSDisable();
        if(!OSMessageQRead(Rx2, a)){
            OSMessageQWrite(Tx2, a);
        }
        OSEnable();
    }
}
