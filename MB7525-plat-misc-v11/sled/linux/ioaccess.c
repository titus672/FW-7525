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
 * MB-7551 Version V1.0
 *
 * The IO interface for Status LED is connected to SOC GPIO 36 21 for SUS GPIO.
 * Refer to SOC datasheet for details.
 * The truth table is defined as below:
 *      GPO5  GPO22  LED_STATUS
 *      0       0       OFF
 *      1       0       Green
 *      0       1       Red
 *      1       1       OFF
 *------------------------------------------------------------------------------
*/

/*
 * Device Depend Definition : PCH Cougar Point
*/

#define PCH_GPIO_BASE_ADDRESS	0x500
#define PCH_GPIO_05_BIT	0x00000020
#define PCH_GPIO_22_BIT	0x00400000	

void sled_gpio_init(void)
{
        unsigned long tmp;
//Use GPIO5        
	tmp = inportl(PCH_GPIO_BASE_ADDRESS + 0x80);
	tmp |= (PCH_GPIO_05_BIT);	
	outportl(PCH_GPIO_BASE_ADDRESS + 0x80, tmp);	

//Use GPIO22
	tmp = inportl(PCH_GPIO_BASE_ADDRESS + 0x80);
	tmp |= (PCH_GPIO_22_BIT);
	outportl(PCH_GPIO_BASE_ADDRESS + 0x80, tmp);	

//Set GPO5	        
	tmp = inportl(PCH_GPIO_BASE_ADDRESS + 0x84);	
	tmp &= ~(PCH_GPIO_05_BIT);
	outportl(PCH_GPIO_BASE_ADDRESS + 0x84, tmp);	
	
//Set GP036
	tmp = inportl(PCH_GPIO_BASE_ADDRESS + 0x84);	
	tmp &= ~(PCH_GPIO_22_BIT);
	outportl(PCH_GPIO_BASE_ADDRESS + 0x84, tmp);	
        return;
}
void set_led_off(void)
{
        unsigned long tmp;
//The GPO5 write "L"        
	tmp = inportl(PCH_GPIO_BASE_ADDRESS + 0x88);	
	tmp &= ~(PCH_GPIO_05_BIT);	
	outportl(PCH_GPIO_BASE_ADDRESS + 0x88, tmp);
			
//The GPO22 write "L"        	
	tmp = inportl(PCH_GPIO_BASE_ADDRESS + 0x88);	
	tmp &= ~(PCH_GPIO_22_BIT);
	outportl(PCH_GPIO_BASE_ADDRESS + 0x88, tmp);
        return;
}

void set_led_green(void)
{
        unsigned long tmp;
//Write GP05 ->1  GPO22 ->0        
	tmp = inportl(PCH_GPIO_BASE_ADDRESS + 0x88);	
	tmp &= ~PCH_GPIO_22_BIT;	
	outportl(PCH_GPIO_BASE_ADDRESS + 0x88, tmp);		
	
	tmp = inportl(PCH_GPIO_BASE_ADDRESS + 0x88);	
	tmp |= PCH_GPIO_05_BIT;	
	outportl(PCH_GPIO_BASE_ADDRESS + 0x88, tmp);	
        return;
}
void set_led_red(void)
{
        unsigned long tmp;
//Write GP05 ->0  GPO22 ->1                
	tmp = inportl(PCH_GPIO_BASE_ADDRESS + 0x88);	
	tmp |= PCH_GPIO_22_BIT;	
	outportl(PCH_GPIO_BASE_ADDRESS + 0x88, tmp);		
	
	tmp = inportl(PCH_GPIO_BASE_ADDRESS + 0x88);	
	tmp &= ~PCH_GPIO_05_BIT;
	outportl(PCH_GPIO_BASE_ADDRESS + 0x88, tmp);
        return;
}


#endif
