#include "rtos.h"

void test1(void);
void test2(void);
void test3(void);

P_MSGQ Rx1;
P_MSGQ Tx1;
P_MSGQ Rx2;
P_MSGQ Tx2;
char memtest[5000];
int task1_stack[32];
int task2_stack[32];
int task3_stack[32];

int main(void){
    OSInit(100, memtest, 5000);  // Time slice = 1ms
    OSTaskCreate(test1, 0, 30, 0, (char *)task1_stack, sizeof(task1_stack));  
    Rx1 = OSMessageQCreate(5, 5);
    Tx1 = OSMessageQCreate(6, 5);
    OSTaskCreate(test2, 0, 60, 1, (char *)task2_stack, sizeof(task2_stack));   
    Rx2 = OSMessageQCreate(7, 5);
    Tx2 = OSMessageQCreate(8, 5);
    OSTaskCreate(test3, 0, 15, 2, (char *)task3_stack, sizeof(task3_stack)); 
    OSMessageQWrite(Rx1, "Hello");
    OSFirstEnable();
    
    // Should not be here
    
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
void test3(void){
    char a[10];
    while(1){        
        OSDisable();
        if(!OSMessageQRead(Tx1, a)){
            OSMessageQWrite(Rx2, a);
        }
        if(!OSMessageQRead(Tx2, a)){
            OSMessageQWrite(Rx1, a);
        }
        OSEnable();        
    }
}
