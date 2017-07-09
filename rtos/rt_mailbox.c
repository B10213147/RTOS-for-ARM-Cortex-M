/*
 * rt_mailbox.c
 *
 *  Created on: July 10, 2017
 *      Author: Harvard Tseng
 */
 
/* Includes ------------------------------------------------------------------*/
#include "rt_mailbox.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

int OS_MBX_Read(OS_MCB *mail, char *data, unsigned int num_bytes){
    return 0;
}

int OS_MBX_Write(OS_MCB *mail, char *data, unsigned int num_bytes){
    for(int i = 0; i < num_bytes; i++){
        int idx = (mail->end + 1) % mail->length;
        if(idx == mail->begin){ return 1; } // Mailbox is full
        mail->data[mail->end] = data[i];
        mail->end = idx;
    }
    return 0;
}
