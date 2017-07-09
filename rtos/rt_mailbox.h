/*
 * rt_mailbox.h
 *
 *  Created on: July 10, 2017
 *      Author: Harvard Tseng
 */
 
#ifndef RT_MAILBOX_H_
#define RT_MAILBOX_H_

#include "rt_TypeDef.h"

int OS_MBX_Read(OS_MCB *mail, char *data, unsigned int num_bytes);
int OS_MBX_Write(OS_MCB *mail, char *data, unsigned int num_bytes);


#endif /* RT_MAILBOX_H_ */
