#include "rtos.h"

int main(void){
    OS_Init(1000, CM_SysTick);  // Time slice = 1ms
    while(1);
    return 0;
}
