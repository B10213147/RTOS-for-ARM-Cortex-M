#include "rtos.h"

void test1(void);
void test2(void);

char memtest[500];
int main(void){
    /*
    OS_Init(1000, CM_SysTick);  // Time slice = 1ms
    OS_Task_Create(test1, 0);    
    OS_Task_Create(__empty, 0);
    OS_Task_Create(test2, 0);    
    OS_Task_Create(__empty, 0);
    OS_Enable();
    */
    //rt_mem_create(0, (char*)memtest, 10);

    rt_mem_create(&system_memory, memtest, 500);
    int *a, *b;
    a = (int*)rt_mem_alloc(&system_memory, 10 * sizeof(int));
    b = (int*)rt_mem_alloc(&system_memory, 5 * sizeof(int));
    rt_mem_free(&system_memory, a);
    int *c = (int*)rt_mem_alloc(&system_memory, 5 * sizeof(int));
    a = (int*)rt_mem_alloc(&system_memory, 10 * sizeof(int));
    rt_mem_free(&system_memory, a);
    rt_mem_free(&system_memory, b);
    rt_mem_free(&system_memory, c);
    while(1){
        

    }
    return 0;
}

void test1(void){

}
void test2(void){

}
