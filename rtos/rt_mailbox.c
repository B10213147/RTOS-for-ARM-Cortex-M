/*
 * rt_mailbox.c
 *
 *  Created on: July 10, 2017
 *      Author: Harvard Tseng
 */
 
/* Includes ------------------------------------------------------------------*/
#include "rt_mailbox.h"
#include "rtos.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Create a mailbox.
  * @param  size: Size in byte.
  * @retval Pointer to mailbox.
  */
P_MAIL rt_mail_create(uint32_t size){
    P_MAIL p_new = NULL;
    char *data;
    OSDisable();
    
    p_new = (P_MAIL)rt_mem_alloc(&system_memory, sizeof(struct mail_blk));
    data = (char *)rt_mem_alloc(&system_memory, size);
    
    if(!p_new || !data){
        rt_mem_free(&system_memory, p_new);
        rt_mem_free(&system_memory, data);
        OSEnable();
        return NULL;
    }
    
    p_new->begin = 0;
    p_new->end = 0;
    p_new->length = size;
    p_new->data = data;
    
    OSEnable();
    return p_new;
}

/**
  * @brief  Delete mailbox.
  * @param  mail: Pointer to mailbox.
  * @retval None
  */
void rt_mail_delete(P_MAIL mail){
    OSDisable();
    rt_mem_free(&system_memory, mail->data);
    rt_mem_free(&system_memory, mail);
    OSEnable();
}

/**
  * @brief  Read data from mailbox.
  * @param  mail: Pointer to mailbox.
  * @param  data: Pointer to a space where read data can put.
  * @param  num_bytes: Number of bytes to read.
  * @retval Number of successfully read bytes.
  */
uint32_t rt_mail_read(P_MAIL mail, char *data, uint32_t num_bytes){
    uint32_t i;
    for(i = 0; i < num_bytes; i++){
        if(mail->begin == mail->end){   
            break;   // Mailbox is empty
        } 
        data[i] = mail->data[mail->begin];
        mail->begin = (mail->begin + 1) % mail->length;
    }
    return i;
}

/**
  * @brief  Write data into mailbox.
  * @param  mail: Pointer to mailbox.
  * @param  data: Pointer to data wait for written.
  * @param  num_bytes: Number of bytes to read.
  * @retval 0 Function succeeded.
  * @retval 1 Function failed.
  */
uint8_t rt_mail_write(P_MAIL mail, char *data, uint32_t num_bytes){
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
