TARGET_NAME=wd_drv
CFILES = drv_main.c ioaccess.c


# Kernel Version Check
KVER  := $(shell uname -r | cut -c1-3 | sed 's/2\.[56]/2\.6/')
#;tadd++Start
KVER3  := $(shell uname -r | cut -c1-1)				
#;tadd++End
# Makefile for a basic kernel module - 2.4.x
KINC=
ifeq ($(KINC),)
KINC  := $(shell uname -r)
endif
CC    := gcc
MODCFLAGS := -DMODULE -D__KERNEL__ -DLINUX -DOLDKERNEL -O -I/usr/src/linux-$(KINC)/include
EXTRA_CFLAGS += -I./../include

# Makefile for a basic kernel module - 2.6.x
obj-m += $(TARGET_NAME).o 
$(TARGET_NAME)-objs := $(CFILES:.c=.o) 
KDIR  := /lib/modules/$(shell uname -r)/build
PWD   := $(shell pwd)

# MODULE -- This tells the header files to give the appropriate
# definitions for a kernel module.
#
# __KERNEL__ -- This tells the header files that this code will
# be run in kernel mode not as part of a user process.
#
# LINUX -- Technically speaking, this is not necessary. However,
# if you ever want to write a serious kernel module which will
# compile on more than one operating system, you'll be happy you
# did. This will allow you to do conditional compilation on the
# parts which are OS dependant.
#
all:
ifeq ($(wildcard /dev/$(TARGET_NAME)),)
	mknod /dev/$(TARGET_NAME) c 241 0
endif

#;tadd++Start
ifeq ($(KVER3),3)
	$(MAKE) -C $(KDIR) M=$(PWD) modules
	-cp $(TARGET_NAME).ko ../bin	
endif
#;tadd++End

ifeq ($(KVER),2.6)
	$(MAKE) -C $(KDIR) M=$(PWD) modules
	-cp $(TARGET_NAME).ko ../bin	
endif
ifeq ($(KVER),2.4)
	$(CC) $(MODCFLAGS) -c -o drv_main.o drv_main.c
	$(CC) $(MODCFLAGS) -c -o ioaccess.o ioaccess.c
	$(LD) $(LDFLAGS) -r drv_main.o ioaccess.o -o $(TARGET_NAME).o
	-cp $(TARGET_NAME).o ../bin	
endif
clean:
ifeq ($(KVER),2.6)
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f Module.markers
	rm -f Module.symvers
	rm -f modules.order
endif
ifeq ($(KVER),2.4)
	rm -f *.o *.ko
endif

