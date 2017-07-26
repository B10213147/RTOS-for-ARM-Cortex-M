#include "rtos.h"

void test1(void);
void test2(void);
void test3(void);

P_MSGQ Rx1;
P_MSGQ Tx1;
P_MSGQ Rx2;
P_MSGQ Tx2;
int memtest[10000];
extern uint32_t MSP_bottom;

int main(void){
    MSP_bottom = __get_MSP();
    OSInit(10, (char *)memtest, sizeof(memtest));  // Time slice = 1ms
    OSTaskCreate(test1, 0, 30, 0); 
    __set_PRIMASK(0x0U);    
    Rx1 = OSMessageQCreate(5, 5);
    Tx1 = OSMessageQCreate(6, 5);
    OSTaskCreate(test2, 0, 60, 1);  
    __set_PRIMASK(0x0U);  
    Rx2 = OSMessageQCreate(7, 5);
    Tx2 = OSMessageQCreate(8, 5);
    OSTaskCreate(test3, 0, 15, 2); 
    __set_PRIMASK(0x0U);
    OSMessageQWrite(Rx1, "Hello");
    OSFirstEnable();
    
    // Should not be here
    
    return 0;    
}
P_MSGQ foo;
void test1(void){
    char a[10];
    while(1){
        if(!OSMessageQRead(Rx1, a)){
            OSMessageQWrite(Tx1, a);
            OSMessageQDistroy(foo);
        }        
    }
}
void test2(void){
    char a[10];
    while(1){
        if(!OSMessageQRead(Rx2, a)){
            OSMessageQWrite(Tx2, a);
            foo = OSMessageQCreate(5, 5);
        }      
    }
}
void test3(void){
    char *a;
    while(1){
        a = (char *)OSmalloc(10);
        if(!OSMessageQRead(Tx1, a)){
            OSMessageQWrite(Rx2, a);
        }
        if(!OSMessageQRead(Tx2, a)){
            OSMessageQWrite(Rx1, a);
        }
        OSfree(a);
    }
}
