/*
 * rt_mailbox.h
 *
 *  Created on: July 10, 2017
 *      Author: Harvard Tseng
 */
 
#ifndef RT_MAILBOX_H_
#define RT_MAILBOX_H_

#include "rt_TypeDef.h"

uint32_t OS_MBX_Read(P_MCB mail, char *data, uint32_t num_bytes);
uint8_t OS_MBX_Write(P_MCB mail, char *data, uint32_t num_bytes);


#endif /* RT_MAILBOX_H_ */
