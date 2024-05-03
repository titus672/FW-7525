/*******************************************************************************

  drv_main.c : Main code of FreeBSD driver for Lanner platform Watchdog/Bypass
                program

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


#include <sys/types.h>
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/bus.h>
#include <sys/malloc.h>
#include <sys/kernel.h>
#include <sys/errno.h>
#include <sys/conf.h>
#include <sys/module.h>

#include <sys/time.h>
#include <sys/proc.h>
#include <sys/unistd.h>
#include <sys/fcntl.h>
//#include <sys/ioctl.h>
#include <sys/ioccom.h>
#include <sys/sysctl.h>
#include <sys/signalvar.h>

#include <sys/uio.h>
#include <machine/bus.h>
#include <machine/resource.h>
#include <sys/rman.h>
#include "../include/wd_ioctl.h"
#include "../include/ioaccess.h"
#include "../include/version.h"


static struct cdev *wd_dev;
static int isopen;
static int watchdog_time = 0;

static	d_open_t	wd_open;
static	d_close_t	wd_close;
static	d_ioctl_t	wd_ioctl;

static struct cdevsw wd_cdevsw = {
	.d_version	= D_VERSION,
	.d_name		= "wd",
	.d_flags	= D_NEEDGIANT,
	.d_open		= wd_open,  /* open */
	.d_close	= wd_close, /* close */
	.d_ioctl	= wd_ioctl  /* ioctl */
};

static int wd_open(struct cdev *dev, int flags, int devtype, struct thread *td) {
	if(isopen != 0) {
		return (EBUSY);
	}
	isopen = 1;
	return 0;
}

static int wd_close(struct cdev *dev, int flags, int fmt, struct thread *td) {
	
 	isopen=0;
		
	return 0;
}

static int wd_ioctl(struct cdev *dev, u_long cmd, caddr_t data,
	int fflag, struct thread *td)
{
/*
	int *value = (int*)data;
        unsigned char tmp;

        switch(cmd)
        {
                case IOCTL_BTN_GET_STATUS:
                        tmp = get_btn_status();
                        *value = (int)tmp;
                        break;
                default:
                        return -ENODEV;
        }
        return 0;
*/
	int pair_no;
        int arg = (int)*data;

	pair_no = arg;
        switch(cmd)
        {
                case IOCTL_SYSTEM_OFF_BYPASS_STATE:
                        pair_no &= 0xff0;
                        switch(arg & 0x0f)
                        {
                                case SYSTEM_OFF_BYPASS_STATE_ENABLE:
                                        set_bypass_enable_when_system_off(pair_no);
                                        break;

                                case SYSTEM_OFF_BYPASS_STATE_DISABLE:
                                        set_bypass_disable_when_system_off(pair_no);
                                        break;

                                default:
                                        return -ENODEV;
                        }
                        break;

                case IOCTL_RUNTIME_BYPASS_STATE:
                        pair_no &= 0xff0;
                        switch(arg & 0x0f)
                        {
                                case RUNTIME_BYPASS_STATE_ENABLE:
                                        set_runtime_bypass_enable(pair_no);
                                        break;

                                case RUNTIME_BYPASS_STATE_DISABLE:
                                        set_runtime_bypass_disable(pair_no);
                                        break;

                                default:
                                        return -ENODEV;
                        }
                        break;
                /* Set system behavior once watchdog timer expired */
                case IOCTL_SET_WDTO_STATE:
                        switch(arg)
                        {
                                case SET_WDTO_STATE_SYSTEM_RESET:
                                        set_wdto_state_system_reset();
                                        break;
                                case SET_WDTO_STATE_LAN_BYPASS:
                                        set_wdto_state_system_bypass();
                                        break;
                                default:
                                        return -ENODEV;
                        }
                        break;
                /* Set the watchdog timer */
                case IOCTL_SET_WDTO_TIMER:
                        watchdog_time=(int)arg;
                        if ( (watchdog_time < 0 ) || (watchdog_time > 255))
                        {
                                printf("wd_drv error: Invalid time value\n");
                                watchdog_time=0;
                                return -ENODEV;
                        }
                        break;
                /* Start/Stop watchdog timer */
                case IOCTL_START_STOP_WDT:
                        switch (arg)
                        {
                                case START_WDT:
                                        start_watchdog_timer(watchdog_time);
                                        break;
                                case STOP_WDT:
                                        stop_watchdog_timer();
                                        break;
                                default:
                                        return -ENODEV;
                        }
                        break;
                default:
                        return -ENODEV;
        }
        return 0;


}

static int
wd_loader(struct module *m, int what, void *arg)
{
    int err = 0;

    switch (what) {
    case MOD_LOAD:                /* kldload */
        wd_dev = make_dev(&wd_cdevsw,
            0,
            UID_ROOT,
            GID_WHEEL,
            0600,
            "wd_drv");
        /* kmalloc memory for use by this driver */
        isopen = 0;
	wd_gpio_init();
	printf("Lanner "PLATFORM_NAME" Watchdog/Bypass Driver "CODE_VERSION" loaded\n");
        break;
    case MOD_UNLOAD:
        destroy_dev(wd_dev);
	printf("Lanner "PLATFORM_NAME" Watchdog/Bypass Driver "CODE_VERSION" unloaded\n");
        break;
    default:
        err = EOPNOTSUPP;
        break;
    }
    return(err);
}

DEV_MODULE(wd, wd_loader, NULL);

