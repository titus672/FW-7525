/*******************************************************************************

  ioaccess.h : function declaration file

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


#ifndef DJGPP

#ifdef __FreeBSD__

#define inportb(x) inb(x)
#define outportb(x, y) outb(x, y)
#define inportl(x) inl(x)
#define outportl(x, y) outl(x, y)

#else

#define inportb(x) inb_p(x)
#define outportb(x, y) outb_p(y, x)
#define inportl(x) inl_p(x)
#define outportl(x, y) outl_p(y, x)

#endif
#endif


void enter_w83627_config(void);
void exit_w83627_config(void);
unsigned char read_w83627_reg(int LDN, int reg);
void write_w83627_reg(int LDN, int reg, int value);

void start_watchdog_timer(int watchdog_time);
void stop_watchdog_timer(void);
int wd_gpio_init(void);
void set_bypass_enable_when_system_off(unsigned long pair_no);
void set_bypass_disable_when_system_off(unsigned long pair_no);
void set_runtime_bypass_enable(unsigned long pair_no);
void set_runtime_bypass_disable(unsigned long pair_no);
void set_wdto_state_system_reset(void);
void set_wdto_state_system_bypass(void);


/* Pair definition */
#define BYPASS_PAIR_1     (0x1 << 4)
#define BYPASS_PAIR_2     (0x2 << 4)
#define BYPASS_PAIR_3     (0x4 << 4)
#define BYPASS_PAIR_4     (0x8 << 4)
#define BYPASS_PAIR_5     (0x10 << 4)
#define BYPASS_PAIR_6     (0x20 << 4)

/* Set bypass enable/disable in runtime */
#define RUNTIME_BYPASS_STATE_ENABLE     0
#define RUNTIME_BYPASS_STATE_DISABLE    1

/* Set bypass enable/disable when system off*/
#define SYSTEM_OFF_BYPASS_STATE_ENABLE  0
#define SYSTEM_OFF_BYPASS_STATE_DISABLE 1

/* Start/Stop Watchdog timer */
#define START_WDT                       0
#define STOP_WDT                        1

/* Set the status while watchdog timeout */
#define SET_WDTO_STATE_LAN_BYPASS       0
#define SET_WDTO_STATE_SYSTEM_RESET     1






