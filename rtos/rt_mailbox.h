/*
 * rt_mailbox.h
 *
 *  Created on: July 10, 2017
 *      Author: Harvard Tseng
 */
 
#ifndef RT_MAILBOX_H_
#define RT_MAILBOX_H_

#include "rt_TypeDef.h"

P_MAIL rt_mail_create(uint32_t size);
void rt_mail_delete(P_MAIL mail);
uint32_t rt_mail_read(P_MAIL mail, char *data, uint32_t num_bytes);
uint8_t rt_mail_write(P_MAIL mail, char *data, uint32_t num_bytes);

#endif /* RT_MAILBOX_H_ */
