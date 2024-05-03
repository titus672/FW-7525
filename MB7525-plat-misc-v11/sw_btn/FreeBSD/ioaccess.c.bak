/*******************************************************************************

  ioaccess.c: IO access code for Lanner Platfomr Reset Button program

  Lanner Platform Miscellaneous Utility
  Copyright(c) 2010 Lanner Electronics Inc.
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:
  1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer,
     without modification.
  2. Redistributions in binary form must reproduce at minimum a disclaimer
     similar to the "NO WARRANTY" disclaimer below ("Disclaimer") and any
     redistribution must be conditioned upon including a substantially
     similar Disclaimer requirement for further binary redistribution.
  3. Neither the names of the above-listed copyright holders nor the names
     of any contributors may be used to endorse or promote products derived
     from this software without specific prior written permission.

  Alternatively, this software may be distributed under the terms of the
  GNU General Public License ("GPL") version 2 as published by the Free
  Software Foundation.

  NO WARRANTY
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF NONINFRINGEMENT, MERCHANTIBILITY
  AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
  THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR SPECIAL, EXEMPLARY,
  OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
  IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
  THE POSSIBILITY OF SUCH DAMAGES.

*******************************************************************************/



#include "../include/config.h"


#ifdef DJGPP

/* standard include file */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
/* For DOS DJGPP */
#include <dos.h>
#include <inlines/pc.h>

#else //DJGPP
/* For Linux */


#ifdef DIRECT_IO_ACCESS
/* For Linux direct io access code */
/* standard include file */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#if defined(LINUX_ENV)
#include <sys/io.h>
#endif

#if defined(FreeBSD_ENV)
#include <machine/cpufunc.h>
#endif

#include <time.h>
#include <stdint.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#define delay(x) usleep(x)
#endif


#ifdef MODULE

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/io.h>
#include <linux/delay.h>

#undef delay
#define delay(x) mdelay(x)
#undef fprintf
#define fprintf(S, A)  printk(A)

#endif //MODULE

#ifdef KLD_MODULE

#include <sys/types.h>
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/malloc.h>
#include <sys/kernel.h>
#include <sys/bus.h>
#include <sys/errno.h>

#include <machine/bus.h>
#include <machine/resource.h>

#endif

#endif

/* local include file */
#include "../include/ioaccess.h"

#if (defined(MODULE) || defined(DIRECT_IO_ACCESS) || defined(KLD_MODULE))


/*
 * Platform Depend GPIOs for Reset Button
 */

/*
 *------------------------------------------------------------------------------
 * MB-7537 Version V 1.0
 *
 * The IO interface for Reset Button is connected to Intel ICH8-M GPIO 20.
 * Refer to Intel ICH8-M datasheet for details.
 * The high/low definition is as below:
 *      GPIO20  Button Status
 *     --------------------------
 *      1       Button Pressed
 *      0       Button Released
 *------------------------------------------------------------------------------
*/

/*
 * Device Depend Definition : Intel ICH8-M chipset
*/
#define SB_GPIO_PORT_BASE       0x500
#define SB_GPIO_PORT_USE_SEL    0x2
#define SB_GPIO_PORT_IO_SEL     0x6
#define SB_GPIO_PORT_OFFSET     0xE
#define SB_GPIO_PORT_REG        SB_GPIO_PORT_BASE+SB_GPIO_PORT_OFFSET

#define GPIO20_BIT              (1 << 4)

#define STATUS_BITMASK  GPIO20_BIT
#define STATUS_PRESSED  0
#define STATUS_RELEASED GPIO20_BIT

/*
 * Return value:
 * 	0: BUTTON Pressed
 * 	1: BUTTON Released
 */ 	
unsigned char get_btn_status(void)
{
        return  ( inportb(SB_GPIO_PORT_REG) & STATUS_BITMASK);
}

void btn_gpio_init(void)
{
        unsigned char tmp;
        /* platform initial code */

        /* Reset button initial function */

        /* select GPIO function */
        tmp=inportb(SB_GPIO_PORT_BASE+SB_GPIO_PORT_USE_SEL);
        tmp |= GPIO20_BIT;
        outportb(SB_GPIO_PORT_BASE+SB_GPIO_PORT_USE_SEL, tmp);

        /* set to input mode */
        tmp=inportb(SB_GPIO_PORT_BASE+SB_GPIO_PORT_IO_SEL);
        tmp |= GPIO20_BIT;
        outportb(SB_GPIO_PORT_BASE+SB_GPIO_PORT_IO_SEL, tmp);
	
	return;
}


#endif
