/*******************************************************************************

  wd_tst.c: main application for Lanner platform Watchdog/Bypass program

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
#include "../include/version.h"

/* standard include file */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifdef DJGPP

/* For DOS DJGPP */
#include <dos.h>
#include <inlines/pc.h>

#ifndef DIRECT_IO_ACCESS
#error ***Error: define DIRECT_IO_ACCESS in config.h for DOS ***
#endif

#else

/* For Linux */
#if defined(LINUX_ENV)
#include <sys/io.h>
#endif

#if defined(FreeBSD_ENV)
#define SET_IOPL() (iofl=open("/dev/io",000))
#define RESET_IOPL() close(iofl)
#include <machine/cpufunc.h>
#endif

#include <time.h>
#include <stdint.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include "../include/wd_ioctl.h"

#define delay(x) usleep(x)

#ifdef DIRECT_IO_ACCESS
#warning ***** Note: You build with DIRECT_IO_ACCESS defined *****
#warning ***** Note: undefine this to build for driver code *****
#endif

#endif

/* local include file */
#include "../include/ioaccess.h"

#ifndef BYPASS_PAIR_NUMBER
#error "You should defined PAIR_NUMBER in include/config.h"
#endif

#if BYPASS_PAIR_NUMBER == 1
char pair_str[16]="[1]";
#endif

#if BYPASS_PAIR_NUMBER == 2
char pair_str[16]="[1|2]";
#endif

#if BYPASS_PAIR_NUMBER == 3
char pair_str[16]="[1|2|3]";
#endif

#if BYPASS_PAIR_NUMBER == 4
char pair_str[16]="[1|2|3|4]";
#endif

#if BYPASS_PAIR_NUMBER == 5
char pair_str[16]="[1|2|3|4|5]";
#endif

void print_usage(char* argv0)
{
#if BYPASS_PAIR_NUMBER > 0
        printf("%s --srbe %s (Set Pair %s Runtime Bypass Enabled)\n", argv0, pair_str, pair_str);
        printf("%s --srbd %s (Set Pair %s Runtime Bypass Disabled)\n", argv0, pair_str, pair_str);
        printf("%s --sobe %s (Set Pair %s Off-mode Bypass Enabled)\n", argv0, pair_str, pair_str);
        printf("%s --sobd %s (Set Pair %s Off-mode Bypass Disabled)\n", argv0, pair_str, pair_str);
        printf("%s --swtsb (Set Watchdog Timeout State to Bypass)\n", argv0);
        printf("%s --swtsr (Set Watchdog Timeout State to Reset)\n", argv0);
#endif
#if ( defined(DIRECT_IO_ACCESS) ) 
        printf("%s --swt xxx (Set Watchdog Timer 1-255 seconds and start to count-down)\n", argv0);
#else 
        printf("%s --swt xxx (Set Watchdog Timer 1-255 seconds)\n", argv0);
        printf("%s --start (Start Watchdog Timer)\n", argv0);
#endif
        printf("%s --stop (Stop Watchdog Timer)\n", argv0);
}


int main( int argc, char** argv )
{
        int devfd;
        int value;


	fprintf(stdout, "=== Lanner platform miscellaneous utility ===\n");;
	fprintf(stdout, PLATFORM_NAME" Watchdog/Bypass "CODE_VERSION"\n\n");;
        if ( argc < 2) {
                print_usage(argv[0]);
                return -1;
        }

#if ( defined(DIRECT_IO_ACCESS) && !defined(DJGPP) ) 
#if defined(LINUX_ENV)
	iopl(3);
#endif
#if (defined(FreeBSD_ENV))
       int iofl;
       SET_IOPL();
#endif
#endif

#ifdef DIRECT_IO_ACCESS
	wd_gpio_init();
#else
        devfd = open("/dev/wd_drv", O_RDONLY);
        if(devfd == -1)
        {
                printf("Can't open /dev/wd_drv\n");
                return -1;
        }
#endif

/******** set watchdog timer count ***************************************/
	if ( !strcmp(argv[1], "--swt")) {
                int tmp;
                if (argc !=3) {
                        printf("No timer input, program terminated\n");
                        close(devfd);
                        exit -1;
                }
                tmp = atoi(argv[2]);
                if ( (tmp < 1) || (tmp > 255)) {
                        printf("Wrong timer value, please input within (1~255)\n");
                        close(devfd);
                        exit -1;
                }
                printf("Set Watchdog Timer....");
#if defined(DIRECT_IO_ACCESS)
	    	start_watchdog_timer(tmp);
            	printf("OK\n");
#else
                if( ioctl(devfd, IOCTL_SET_WDTO_TIMER, &tmp) != 0)
                        printf("Fail\n");
                else
                        printf("OK\n");
#endif
        }
/******** Start watchdog timer *******************************************/
#if !defined(DIRECT_IO_ACCESS)
        else if ( !strcmp(argv[1], "--start")) {
                printf("Start Watchdog Timer....");
		value = START_WDT;
                if( ioctl(devfd, IOCTL_START_STOP_WDT, &value) != 0)
                        printf("Fail\n");
                else
                        printf("OK\n");
        }
#endif
/******** Stop watchdog timer ********************************************/
        else if ( !strcmp(argv[1], "--stop")) {
                printf("Stop Watchdog Timer....");
#if defined(DIRECT_IO_ACCESS)
	    	stop_watchdog_timer();
            	printf("OK\n");
#else
		value = STOP_WDT;
                if( ioctl(devfd, IOCTL_START_STOP_WDT, &value) != 0)
                        printf("Fail\n");
                else
                        printf("OK\n");
#endif
        }

#if BYPASS_PAIR_NUMBER > 0
/******** Set lan-bypass enable while watchdog timer expired *************/
        else if ( !strcmp(argv[1], "--swtsb")) {
                printf("Set Watchdog Timeout State to Lan Bypass....");
#if defined(DIRECT_IO_ACCESS)
	    	set_wdto_state_system_bypass();
            	printf("OK\n");
#else
		value = SET_WDTO_STATE_LAN_BYPASS;
                if( ioctl(devfd, IOCTL_SET_WDTO_STATE, &value) != 0)
                        printf("Fail\n");
                else
                        printf("OK\n");
#endif
        }
/******** Set system reset while watchdog timer expired ******************/
        else if ( !strcmp(argv[1], "--swtsr")) {
                printf("Set Watchdog Timeout State to System Reset....");
#if defined(DIRECT_IO_ACCESS)
	    	set_wdto_state_system_reset();
            	printf("OK\n");
#else
		value = SET_WDTO_STATE_SYSTEM_RESET;
                if( ioctl(devfd, IOCTL_SET_WDTO_STATE, &value) != 0)
                        printf("Fail\n");
                else
                        printf("OK\n");
#endif
        }

/******** Set runtime bypass enable **************************************/
        else if ( (!strcmp(argv[1], "--srbe")) && (argc==3)) {
            if ( !strcmp(argv[2],"1") ) {
                value = BYPASS_PAIR_1;
#if BYPASS_PAIR_NUMBER > 1
            } else if (!strcmp(argv[2],"2") ) {
                value = BYPASS_PAIR_2;
#if BYPASS_PAIR_NUMBER > 2
            } else if (!strcmp(argv[2],"3") ) {
                value = BYPASS_PAIR_3;
#if BYPASS_PAIR_NUMBER > 3
            } else if (!strcmp(argv[2],"4") ) {
                value = BYPASS_PAIR_4;
#if BYPASS_PAIR_NUMBER > 4
            } else if (!strcmp(argv[2],"5") ) {
                value = BYPASS_PAIR_5;
#endif	//>4
#endif	//>3
#endif	//>2
#endif	//>1
            } else {
                printf("Wrong pair number\n");
                return -1;
            }
            printf("Set Runtime Bypass Enable....");
#if defined(DIRECT_IO_ACCESS)
	    set_runtime_bypass_enable(value);
            printf("OK\n");
#else
	    value |= RUNTIME_BYPASS_STATE_ENABLE;
            if( ioctl(devfd, IOCTL_RUNTIME_BYPASS_STATE, &value) != 0)
                    printf("Fail\n");
            else
                    printf("OK\n");
#endif //DIRECT_IO_ACCESS
        } //--srbe
/******** Set runtime bypass enable **************************************/
        else if ( (!strcmp(argv[1], "--srbd")) && (argc==3)) {
            if ( !strcmp(argv[2],"1") ) {
                value = BYPASS_PAIR_1;
#if BYPASS_PAIR_NUMBER > 1
            } else if (!strcmp(argv[2],"2") ) {
                value = BYPASS_PAIR_2;
#if BYPASS_PAIR_NUMBER > 2
            } else if (!strcmp(argv[2],"3") ) {
                value = BYPASS_PAIR_3;
#if BYPASS_PAIR_NUMBER > 3
            } else if (!strcmp(argv[2],"4") ) {
                value = BYPASS_PAIR_4;
#if BYPASS_PAIR_NUMBER > 4
            } else if (!strcmp(argv[2],"5") ) {
                value = BYPASS_PAIR_5;
#endif  //>4
#endif  //>3
#endif  //>2
#endif  //>1
            } else {
                printf("Wrong pair number\n");
                return -1;
            }
            printf("Set Runtime Bypass Disable....");
#if defined(DIRECT_IO_ACCESS)
            set_runtime_bypass_disable(value);
            printf("OK\n");
#else
	    value |= RUNTIME_BYPASS_STATE_DISABLE;
            if( ioctl(devfd, IOCTL_RUNTIME_BYPASS_STATE, &value) != 0)
                    printf("Fail\n");
            else
                    printf("OK\n");
#endif //DIRECT_IO_ACCESS
        } //--srbd

/******** Set system-off bypass enable ***********************************/
        else if ( (!strcmp(argv[1], "--sobe")) && (argc==3)) {
            if ( !strcmp(argv[2],"1") ) {
                value = BYPASS_PAIR_1;
#if BYPASS_PAIR_NUMBER > 1
            } else if (!strcmp(argv[2],"2") ) {
                value = BYPASS_PAIR_2;
#if BYPASS_PAIR_NUMBER > 2
            } else if (!strcmp(argv[2],"3") ) {
                value = BYPASS_PAIR_3;
#if BYPASS_PAIR_NUMBER > 3
            } else if (!strcmp(argv[2],"4") ) {
                value = BYPASS_PAIR_4;
#if BYPASS_PAIR_NUMBER > 4
            } else if (!strcmp(argv[2],"5") ) {
                value = BYPASS_PAIR_5;
#endif  //>4
#endif  //>3
#endif  //>2
#endif  //>1
            } else {
                printf("Wrong pair number\n");
                return -1;
            }
            printf("Set System-Off Bypass Enable....");
#if defined(DIRECT_IO_ACCESS)
            set_bypass_enable_when_system_off(value);
            printf("OK\n");
#else
	    value |= SYSTEM_OFF_BYPASS_STATE_ENABLE;
            if( ioctl(devfd, IOCTL_SYSTEM_OFF_BYPASS_STATE, &value) != 0)
                    printf("Fail\n");
            else
                    printf("OK\n");
#endif //DIRECT_IO_ACCESS
        } //--sobe

/******** Set system-off bypass disable **********************************/
        else if ( (!strcmp(argv[1], "--sobd")) && (argc==3)) {
            if ( !strcmp(argv[2],"1") ) {
                value = BYPASS_PAIR_1;
#if BYPASS_PAIR_NUMBER > 1
            } else if (!strcmp(argv[2],"2") ) {
                value = BYPASS_PAIR_2;
#if BYPASS_PAIR_NUMBER > 2
            } else if (!strcmp(argv[2],"3") ) {
                value = BYPASS_PAIR_3;
#if BYPASS_PAIR_NUMBER > 3
            } else if (!strcmp(argv[2],"4") ) {
                value = BYPASS_PAIR_4;
#if BYPASS_PAIR_NUMBER > 4
            } else if (!strcmp(argv[2],"5") ) {
                value = BYPASS_PAIR_5;
#endif  //>4
#endif  //>3
#endif  //>2
#endif  //>1
            } else {
                printf("Wrong pair number\n");
                return -1;
            }
            printf("Set System-Off Bypass Disable....");
#if defined(DIRECT_IO_ACCESS)
            set_bypass_disable_when_system_off(value);
            printf("OK\n");
#else
	    value |= SYSTEM_OFF_BYPASS_STATE_DISABLE;
            if( ioctl(devfd, IOCTL_SYSTEM_OFF_BYPASS_STATE, &value) != 0)
                    printf("Fail\n");
            else
                    printf("OK\n");
#endif //DIRECT_IO_ACCESS
        } //--sobe

#endif	//>0

	else {
		printf("<<<Error: Unknown Parameter>>>\n");
                print_usage(argv[0]);
		return -1;
	}

#ifdef DIRECT_IO_ACCESS
#if (defined(FreeBSD_ENV))
       RESET_IOPL();
#endif
#else
        close(devfd);
#endif
        return 0;

}

