#ifndef _DEV_TWIF_H_
#define _DEV_TWIF_H_

/*
 * Copyright (C) 2001-2003 by egnite Software GmbH. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the names of
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY EGNITE SOFTWARE GMBH AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL EGNITE
 * SOFTWARE GMBH OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * For additional information see http://www.ethernut.de/
 */

/*
 * $Log$
 * Revision 1.2  2004/12/29 07:28:56  psionic
 * - Added basic twif "substitute" implementation for unix-side testing of
 *   i2c stuff. Don't expect sending or receiving data to actually work.
 *
 *   This exists solely for logging purposes, however TwIOCtl properly
 *   supports setting/getting of speed and addres.
 *
 * Revision 1.1  2004/12/29 06:51:48  psionic
 * - twif.h stolen from the projects twif implementation, used for stubs
 *
 * Revision 1.1.1.1  2003/05/09 14:41:09  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.2  2003/03/31 14:53:23  harald
 * Prepare release 3.1
 *
 */

#include <sys/types.h>

#define TWI_SETSPEED		0x0401	/*!< \brief Set transfer speed. */
#define TWI_GETSPEED		0x0402	/*!< \brief Query transfer speed. */
#define TWI_SETSLAVEADDRESS	0x0403	/*!< \brief Set local slave address. */
#define TWI_GETSLAVEADDRESS	0x0404	/*!< \brief Query local slave address. */
#define TWI_SETSTATUS		0x0409	/*!< \brief Set status. */
#define TWI_GETSTATUS		0x040a	/*!< \brief Query status. */


#define TWERR_OK		0	/*!< \brief No error occured. */
#define TWERR_TIMEOUT		1	/*!< \brief Interface timeout. */
#define TWERR_BUS		2	/*!< \brief Bus error. */
#define TWERR_IF_LOCKED		3	/*!< \brief Interface locked. */
#define TWERR_SLA_NACK		4	/*!< \brief No slave response. */
#define TWERR_DATA_NACK		5	/*!< \brief Data not acknowledged. */


#define TWSLA_MIN		17	/*!< \brief Lowest slave address.
					 * Addresses below are reserved 
					 * for special purposes.
					 */
#define TWSLA_MAX		79	/*!< \brief Lowest slave address.
					 * Addresses above are reserved 
					 * for special purposes.
					 */
#define TWSLA_BCAST		0	/*!< \brief Broadcast slave address. */
#define TWSLA_HOST		16	/*!< \brief Host slave address. */
#define TWSLA_DEFAULT		193	/*!< \brief Default slave address. */

int TwInit(u_char sla);
int TwIOCtl(int req, void *conf);

int TwMasterTransact(u_char sla, void *txdata, u_short txlen, void *rxdata, u_short rxsiz, u_long tmo);
int TwMasterError(void);

int TwSlaveListen(u_char *sla, void *rxdata, u_short rxsiz, u_long tmo);
int TwSlaveRespond(void *txdata, u_short txlen, u_long tmo);
int TwSlaveError(void);


#endif
