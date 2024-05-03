/*******************************************************************************

  ioaccess.c: IO access code for Lanner Platfomr Digital IO program

  Lanner Platform Miscellaneous Utility
  Copyright(c) 2010- 2011 Lanner Electronics Inc.
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
 *------------------------------------------------------------------------------
 * MB-7525 Version V1.0
 *
 * The IO interface for Digital DIO is connected to Winbond SIO 83627DHG.
 * Platform provide 4 digital input and 4 digital output. 
 * GPIO21 33 54 57 as input function, GPIO20 32 53 56 as output function
 * Refer to Winbond 83627 datasheet for details.
 * The truth table is defined as below:
 * DIO GPIO pins as follows:
 *		IN		OUT
 * DIO		GP21		GP20
 * DIO		GP33		GP32
 * DIO		GP54		GP53
 * DIO		GP57		GP56
 *-----------------------------------------------------------------------------------
 */

/*
 * Device Depend Definition : Winbond 83627 SIO
*/
#define INDEX_PORT      0x2E
#define DATA_PORT       0x2F

#define GPIO2X		1
#define GPIO20_BIT	(1 << 0) 
#define GPIO21_BIT	(1 << 1)
#define GPIO_GPIO20_GPIO21_MASK (GPIO20_BIT | GPIO21_BIT)

#define GPIO3X		2
#define GPIO32_BIT	(1 << 2) 
#define GPIO33_BIT	(1 << 3)
#define GPIO_GPIO32_GPIO33_MASK (GPIO32_BIT | GPIO33_BIT)

#define GPIO4X		4
#define GPIO46_BIT	(1 << 6)
#define GPIO47_BIT	(1 << 7)
#define GPIO_GPIO46_GPIO47_MASK (GPIO46_BIT | GPIO47_BIT)

#define GPIO5X		8
#define GPIO53_BIT	(1 << 3)
#define GPIO54_BIT	(1 << 4)
#define GPIO56_BIT	(1 << 6)
#define GPIO57_BIT	(1 << 7)
#define GPIO_GPIO53_GPIO54_MASK (GPIO53_BIT | GPIO54_BIT)
#define GPIO_GPIO56_GPIO57_MASK (GPIO56_BIT | GPIO57_BIT)

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

void dio_gpio_init(void)
{
        unsigned char tmp;
	/* Enable GPIO 2x 4x 5x function */
	tmp=read_w83627_reg(0x09, 0x30);
	tmp = tmp | (GPIO2X + GPIO3X + GPIO5X);
	write_w83627_reg(0x09, 0x30, tmp);
	
	/* set GPO 20 and GPI21 */
	tmp=read_w83627_reg(0x09, 0xE3);
	tmp &= ~(GPIO_GPIO20_GPIO21_MASK);
	tmp |= GPIO21_BIT;
	write_w83627_reg(0x09, 0xE3, tmp);
        
	/* set GPIO 20~21  as uninvert */
	tmp=read_w83627_reg(0x09, 0xE5);
	tmp &= ~(GPIO_GPIO20_GPIO21_MASK);
	write_w83627_reg(0x09, 0xE5, tmp);
        
	/* set GPO 32 and GPI 33 */
	tmp=read_w83627_reg(0x09, 0xF0);
	tmp &= ~(GPIO_GPIO32_GPIO33_MASK);
	tmp |= GPIO33_BIT;
	write_w83627_reg(0x09, 0xF0, tmp);
        
        /* set GPIO 32~33  as uninvert */
	tmp=read_w83627_reg(0x09, 0xF2);
	tmp &= ~(GPIO_GPIO32_GPIO33_MASK);
	write_w83627_reg(0x09, 0xF2, tmp);
	
	/* set GPO 53 and GPI 54 */
	tmp=read_w83627_reg(0x09, 0xE0);
	tmp &= ~(GPIO_GPIO53_GPIO54_MASK);
	tmp |= GPIO54_BIT;
	write_w83627_reg(0x09, 0xE0, tmp);
        
        /* set GPIO 53~54  as uninvert */
	tmp=read_w83627_reg(0x09, 0xE2);
	tmp &= ~(GPIO_GPIO53_GPIO54_MASK);
	write_w83627_reg(0x09, 0xE2, tmp);

	/* set GPO 56 and GPI 57 */
	tmp=read_w83627_reg(0x09, 0xE0);
	tmp &= ~(GPIO_GPIO56_GPIO57_MASK);
	tmp |= GPIO57_BIT;
	write_w83627_reg(0x09, 0xE0, tmp);
        
        /* set GPIO 56~57  as uninvert */
	tmp=read_w83627_reg(0x09, 0xE2);
	tmp &= ~(GPIO_GPIO56_GPIO57_MASK);
	write_w83627_reg(0x09, 0xE2, tmp);        
	
        return;
}
void dio_set_output(unsigned char out_value)
{
	unsigned char tmp;

	tmp = read_w83627_reg(0x09,0xE4);
	tmp &= ~GPIO20_BIT;
	tmp |= (out_value & 0x01);
	write_w83627_reg(0x09, 0xE4, tmp);
	
	tmp = read_w83627_reg(0x09,0xF1);
	tmp &= ~GPIO32_BIT;
	tmp |= ((out_value & 0x02) << 1);
	write_w83627_reg(0x09, 0xF1, tmp);
	
	tmp = read_w83627_reg(0x09,0xE1);
	tmp &= ~GPIO53_BIT;
	tmp |= ((out_value & 0x04) << 1);
	write_w83627_reg(0x09, 0xE1, tmp);
	
	tmp = read_w83627_reg(0x09,0xE1);
	tmp &= ~GPIO56_BIT;
	tmp |= ((out_value & 0x08) << 3);
	write_w83627_reg(0x09, 0xE1, tmp);
	
	delay(333);
        return;  
}

unsigned char dio_get_input(void)
{
        unsigned char tmp, tmp1=0;

        tmp=read_w83627_reg(0x09, 0xE4);
        tmp &= GPIO21_BIT;
        tmp1 = tmp >> 1;
        
        tmp=read_w83627_reg(0x09, 0xF1);
        tmp &= GPIO33_BIT;
        tmp1 |= (tmp >> 2);
        
        tmp=read_w83627_reg(0x09, 0xE1);
        tmp &= GPIO54_BIT;
        tmp1 |= (tmp >> 2);
        
        tmp=read_w83627_reg(0x09, 0xE1);
        tmp &= GPIO57_BIT;
        tmp1 |= (tmp >> 4);      
        
        return tmp1;
}

#endif
