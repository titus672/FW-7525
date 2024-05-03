/*******************************************************************************

  wd_ioctl.h : Linux IOCTL code definition file for Lanner Plaform 
               Watchdog/Bypass Program

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

#define MAGIC_CHAR 'W'

#if defined(__FreeBSD__)

#if !defined(KLD_MODULE)
#include <sys/ioctl.h>
#endif

/* Set bypass enable/disable in runtime */
#define IOCTL_RUNTIME_BYPASS_STATE	_IOWR(MAGIC_CHAR, 20, int)

/* Set bypass enable/disable when system off*/
#define IOCTL_SYSTEM_OFF_BYPASS_STATE   _IOWR(MAGIC_CHAR, 21, int)

/* Start/Stop Watchdog timer */
#define IOCTL_START_STOP_WDT            _IOWR(MAGIC_CHAR, 22, int)

/* Set the status while watchdog timeout */
#define IOCTL_SET_WDTO_STATE            _IOWR(MAGIC_CHAR, 23, int)

/* Set the watchdog timer */
#define IOCTL_SET_WDTO_TIMER            _IOWR(MAGIC_CHAR, 24, int)


#else

#include <linux/ioctl.h>

/* Set bypass enable/disable in runtime */
#define IOCTL_RUNTIME_BYPASS_STATE      _IOW(MAGIC_CHAR, 20, int)

/* Set bypass enable/disable when system off*/
#define IOCTL_SYSTEM_OFF_BYPASS_STATE   _IOW(MAGIC_CHAR, 21, int)

/* Start/Stop Watchdog timer */
#define IOCTL_START_STOP_WDT            _IOW(MAGIC_CHAR, 22, int)

/* Set the status while watchdog timeout */
#define IOCTL_SET_WDTO_STATE            _IOW(MAGIC_CHAR, 23, int)

/* Set the watchdog timer */
#define IOCTL_SET_WDTO_TIMER            _IOW(MAGIC_CHAR, 24, int)

#endif



