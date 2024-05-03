
# Set to 1 for direct access IO register in user-mode program.
# This is suitable for DOS environment or evaluation purpose in Linux.
# Set to 0 to leave IO access to driver for noraml operation.
#
DIRECT_IO_ACCESS=0


ifeq ($(DIRECT_IO_ACCESS), 1)

LANNER_DRIVER=0

else

LANNER_DRIVER=1

endif 


ifeq ($(LANNER_DRIVER), 1) 
DIRS = linux src
else
DIRS = src
endif

all: config
	rm -rf linux/ioaccess.c
	ln -s ../src/ioaccess.c linux/ioaccess.c
	for i in $(DIRS); do $(MAKE) -C $$i $@; done

config:
	-rm -rf ./include/config.h
	-echo "//Created by Makefile, do not edit it" >> ./include/config.h
ifeq ($(DIRECT_IO_ACCESS), 1)
	-echo "#define DIRECT_IO_ACCESS" >> ./include/config.h
	-echo "#undef LANNER_DRIVER" >> ./include/config.h
else
	-echo "#undef DIRECT_IO_ACCESS" >> ./include/config.h
	-echo "#define LANNER_DRIVER" >> ./include/config.h
endif	
	-echo "#define LINUX_ENV" >> ./include/config.h

clean:
	for i in $(DIRS); do $(MAKE) -C $$i $@; done
	-rm -f include/config.h
	-rm -rf bin/btn_tst
	-rm -rf bin/btn_drv.ko
	-rm -rf bin/btn_drv.o
	-rm -rf linux/ioaccess.c

.PHONY: all clean config
