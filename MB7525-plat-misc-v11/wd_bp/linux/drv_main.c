/*******************************************************************************

  drv_main.c : Main code of Linux driver for Lanner platform Watchdof/Bypass
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


/* Standard in kernel modules */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/pci.h>
#include <asm/io.h>
#include <linux/delay.h>
//#include <linux/spinlock.h>
#include <asm/uaccess.h>
#include "../include/ioaccess.h"
#include "../include/version.h"
#include "../include/wd_ioctl.h"
//tadd++start
#ifndef LINUX_VERSION_CODE
#include <linux/version.h>
#else
#define KERNEL_VERSION(a,b,c) (((a) << 16) + ((b) << 8) + (c))
#endif
//tadd++end
/*
 * Device Major Number
 */
#define WD_MAJOR 241

/*
 * Is the device opened right now?
 * Prevent to access the device in the same time
 */
static int Device_Open = 0;
static int watchdog_time=0;
//tadd++Start
#if ( LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36) )
static int wd_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
#else
static long wd_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
#endif
//tadd++End
{
	int pair_no;
	int value;

	if( copy_from_user(&value, arg, sizeof(value)))
		return -EFAULT;
        switch(cmd)
        {
                case IOCTL_SYSTEM_OFF_BYPASS_STATE:
                        pair_no = value;
                        pair_no &= 0xff0;
                        switch(value & 0x0f)
                        {
                                case SYSTEM_OFF_BYPASS_STATE_ENABLE:
                                        set_bypass_enable_when_system_off(pair_no);
                                        break;

                                case SYSTEM_OFF_BYPASS_STATE_DISABLE:
                                        set_bypass_disable_when_system_off(pair_no);
                                        break;

                                default:
                                        return -EOPNOTSUPP;
                        }
                        break;

                case IOCTL_RUNTIME_BYPASS_STATE:
                        pair_no = value;
                        pair_no &= 0xff0;
                        switch(value & 0x0f)
                        {
                                case RUNTIME_BYPASS_STATE_ENABLE:
                                        set_runtime_bypass_enable(pair_no);
                                        break;

                                case RUNTIME_BYPASS_STATE_DISABLE:
                                        set_runtime_bypass_disable(pair_no);
                                        break;

                                default:
                                        return -EOPNOTSUPP;
                        }
                        break;
		/* Set system behavior once watchdog timer expired */
                case IOCTL_SET_WDTO_STATE:
                        switch(value)
                        {
                                case SET_WDTO_STATE_SYSTEM_RESET:
                                        set_wdto_state_system_reset();
                                        break;
                                case SET_WDTO_STATE_LAN_BYPASS:
                                        set_wdto_state_system_bypass();
                                        break;
                                default:
                                        return -EOPNOTSUPP;
                        }
                        break;
		/* Set the watchdog timer */
                case IOCTL_SET_WDTO_TIMER:
                        watchdog_time=value;
                        if ( (watchdog_time < 0 ) || (watchdog_time > 255))
                        {
                                printk(KERN_ERR "wd_drv error: Invalid time value\n");
                                watchdog_time=0;
                                return -EOPNOTSUPP;
                        }
                        break;
		/* Start/Stop watchdog timer */
                case IOCTL_START_STOP_WDT:
                        switch (value)
                        {
                                case START_WDT:
                                        start_watchdog_timer(watchdog_time);
                                        break;
                                case STOP_WDT:
                                        stop_watchdog_timer();
                                        break;
                                default:
                                        return -EOPNOTSUPP;
                        }
                        break;
                default:
                        return -EOPNOTSUPP;
        }
        return 0;

}

/*
 * This function is called whenever a process attempts to
 * open the device file
 */
static int wd_open(struct inode * inode, struct file * file)
{
	/* we don't want to talk to two processes at the same time */
	if(Device_Open) return -EBUSY;
	Device_Open++;
	/* Make sure that the module isn't removed while the file
	 * is open by incrementing the usage count (the number of
	 * opened references to the module,if it's zero emmod will
	 * fail)
	 */
	return 0;
}

/*
 * This function is called when a process closes the device file.
 */
static int wd_release(struct inode * inode, struct file * file)
{
	/* ready for next caller */
	Device_Open--;
	/* Decrement the usage count, otherwise once you opened the file
	 * you'll never get rid of the module.
	 */
	return 0;
}

/*
 * This structure will hold the functions to be called
 * when a process does something to the device we created.
 * Since a pointer to this structure is kept in the
 * devices table, it can't be local to init_module.
 * NULL is for unimplemented functions.
 */
#if defined(OLDKERNEL)
static struct file_operations wd_fops = {
	owner:		THIS_MODULE,
	read:		NULL,
	write:		NULL,
	ioctl:		wd_ioctl,
	open:		wd_open,
	release:	wd_release,
};
#else
static const struct file_operations wd_fops = {
//tadd++Start
#if ( LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36) )
	.ioctl		= wd_ioctl,
#else
	.unlocked_ioctl	= wd_ioctl,
#endif
//tadd++end	
//maxwu	.ioctl		= wd_ioctl,
	.open		= wd_open,
	.release	= wd_release,
};
#endif

int wd_init(void)
{
	/*
	 * Register the character device
	 */
	if(register_chrdev(WD_MAJOR, "wd_drv", &wd_fops))
	{
		printk("wd_drv : unable to get major %d\n", WD_MAJOR);
		return -EIO;
	}
	wd_gpio_init();
	printk("Lanner Platform <<%s>> Watchdog/Bypass  Version %s -- loaded\n", PLATFORM_NAME, CODE_VERSION);
	return 0;
}

/*
 * Cleanup - unregister the appropriate file from /proc
 */
void wd_exit(void)
{
	/* Unregister the device */
	unregister_chrdev(WD_MAJOR, "wd_drv");
	/* If there's an error, report it */
	printk("Lanner Platform Watchdog/Bypass Driver -- Unloaded\n");
}

module_init(wd_init);
module_exit(wd_exit);

MODULE_AUTHOR("Lanner SW");
MODULE_DESCRIPTION("Lanner Platform Watchdog/Bypass Driver");
MODULE_LICENSE("Dual BSD/GPL");
