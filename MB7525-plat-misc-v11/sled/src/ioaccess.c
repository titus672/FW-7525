/*******************************************************************************

  ioaccess.c: IO access code for Lanner Platfomr Status LED program

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
 * Platform Depend GPIOs for Status LEDs
 */

/*
 *------------------------------------------------------------------------------
 * MB-7551 Version V1.2
 *
 * The IO interface for Status LED is connected to Winbond SIO 83627 GPIO 37 52
 * Refer to Winbond 83627 datasheet for details.
 * The truth table is defined as below:
 *      GPIO37  GPIO52  LED_STATUS
 *      0       0       OFF
 *      0       1       Red
 *      1       0       GREEN
 *      1       1       OFF
 *------------------------------------------------------------------------------
*/

/*
 * Device Depend Definition : Winbond 83627 SIO
*/
#define INDEX_PORT      0x2E
#define DATA_PORT       0x2F


#define GPIO37_BIT              (1 << 7)
#define GPIO52_BIT              (1 << 2)
#define GPIO_GPIO37_MASK 	GPIO37_BIT
#define GPIO_GPIO52_MASK 	GPIO52_BIT
#define LED_OFF                 0
#define LED_RED                 GPIO52_BIT
#define LED_GREEN             	GPIO37_BIT


void enter_w83627_config(void)
{
        outportb(INDEX_PORT, 0x87); // Must Do It Twice
        outportb(INDEX_PORT, 0x87);
        return;
}

void exit_w83627_config(void)
{
        outportb(INDEX_PORT, 0xAA);
        return;
}

unsigned char read_w83627_reg(int LDN, int reg)
{
        unsigned char tmp = 0;

        enter_w83627_config();
        outportb(INDEX_PORT, 0x07); // LDN Register
        outportb(DATA_PORT, LDN); // Select LDNx
        outportb(INDEX_PORT, reg); // Select Register
        tmp = inportb( DATA_PORT); // Read Register
        exit_w83627_config();
        return tmp;
}


void write_w83627_reg(int LDN, int reg, int value)
{
        enter_w83627_config();
        outportb(INDEX_PORT, 0x07); // LDN Register
        outportb(DATA_PORT, LDN); // Select LDNx
        outportb(INDEX_PORT, reg); // Select Register
        outportb(DATA_PORT, value); // Write Register
        exit_w83627_config();
        return;
}

void sled_gpio_init(void)
{
        unsigned char tmp;

	/* set multi-function for pin 73 gpio52*/
	tmp = read_w83627_reg(0x09, 0x2D);
   	tmp |= 0x04;
   	write_w83627_reg(0x09, 0x2D, tmp);
	
        /* set GP37/52 as Output function */
   	tmp = read_w83627_reg(0x09, 0x30);
   	tmp |= 0x0e;
   	write_w83627_reg(0x09, 0x30, tmp);
        /* set GP37/52 as Output function */
        tmp=read_w83627_reg(0x09, 0xE0);
        tmp &= ~GPIO_GPIO52_MASK;
        write_w83627_reg(0x09, 0xe0, tmp);
        
        tmp=read_w83627_reg(0x09, 0xF0);
        tmp &= ~GPIO_GPIO37_MASK;
        write_w83627_reg(0x09, 0xF0, tmp);

        return;
}
void set_led_off(void)
{
        unsigned char tmp;
        tmp=read_w83627_reg(0x09, 0xF1);
        tmp &= ~GPIO_GPIO37_MASK;
        tmp |= LED_OFF;
        write_w83627_reg(0x09, 0xF1, tmp);
        
        tmp=read_w83627_reg(0x09, 0xe1);
        tmp &= ~GPIO_GPIO52_MASK;
        tmp |= LED_OFF;
        write_w83627_reg(0x09, 0xe1, tmp);
        return;
}

void set_led_red(void)
{
        unsigned char tmp;

        tmp=read_w83627_reg(0x09, 0xF1);
        tmp &= ~GPIO_GPIO37_MASK;
        write_w83627_reg(0x09, 0xF1, tmp);
        
        tmp=read_w83627_reg(0x09, 0xE1);
        tmp &= ~GPIO_GPIO52_MASK;
        tmp |= LED_RED;
        write_w83627_reg(0x09, 0xE1, tmp);
        return;

}
void set_led_green(void)
{
        unsigned char tmp;

        tmp=read_w83627_reg(0x09, 0xF1);
        tmp &= ~GPIO_GPIO37_MASK;
        tmp |= LED_GREEN;
        write_w83627_reg(0x09, 0xF1, tmp);
        
        tmp=read_w83627_reg(0x09, 0xE1);
        tmp &= ~GPIO_GPIO52_MASK;
        write_w83627_reg(0x09, 0xE1, tmp);
        return;
}


#endif
