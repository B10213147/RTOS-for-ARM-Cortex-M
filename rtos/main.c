#include "rtos.h"

void test1(void);
void test2(void);

P_MAIL Rx1;
P_MAIL Tx1;
P_MAIL Rx2;
P_MAIL Tx2;
char memtest[500];

int main(void){
    OSInit(1000, CM_SysTick, memtest, 500);  // Time slice = 1ms
    OSTaskCreate(test1, 0);  
    Rx1 = rt_mail_create(10);
    Tx1 = rt_mail_create(13);
    OSTaskCreate(__empty, 0);
    OSTaskCreate(test2, 0);   
    Rx2 = rt_mail_create(12);
    Tx2 = rt_mail_create(11);
    OSTaskCreate(__empty, 0);
    OSEnable();
    
    rt_mail_write(Rx1, "Hello", 5);
    char a[10];
    while(1){
        OSDisable();
        if(rt_mail_read(Tx1, a, 5) == 5){
            rt_mail_write(Rx2, a, 5);
        }
        OSEnable();
        OSDisable();
        if(rt_mail_read(Tx2, a, 5) == 5){
            rt_mail_write(Rx1, a, 5);
        }
        OSEnable();
    }
    return 0;
    
}

void test1(void){
    char a[10];
    if(rt_mail_read(Rx1, a, 5) == 5){
        rt_mail_write(Tx1, a, 5);
    }
}
void test2(void){
    char a[10];
    if(rt_mail_read(Rx2, a, 5) == 5){
        rt_mail_write(Tx2, a, 5);
    }
}
