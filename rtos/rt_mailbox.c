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
    int diff = mail->begin - mail->end;
    if(diff <= 0){
        if(num_bytes >= mail->length + diff){
            return 1;   // Remaining space is not enough
        }
    }
    else{
        if(num_bytes >= diff){
            return 1;   // Remaining space is not enough
        }
    }
    for(int i = 0; i < num_bytes; i++){
        int idx = (mail->end + 1) % mail->length;
        if(idx == mail->begin){ return 1; } // Mailbox is full
        mail->data[mail->end] = data[i];
        mail->end = idx;
    }
    return 0;
}
