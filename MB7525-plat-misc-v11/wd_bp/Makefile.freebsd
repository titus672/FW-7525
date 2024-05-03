
# Set to 1 for direct access IO register in user-mode program.
# This is suitable for DOS environment or evaluation purpose in Linux.
# Set to 0 to leave IO access to driver for noraml operation.
#
DIRECT_IO_ACCESS=0


.if $(DIRECT_IO_ACCESS) == 1
LANNER_DRIVER=0
.else
LANNER_DRIVER=1
.endif 

.if $(LANNER_DRIVER) == 1 
DIRS = FreeBSD src
.else
DIRS = src
.endif

all: config
	gmake -C src 
.if $(LANNER_DRIVER) == 1
	rm -rf linux/ioaccess.c
	-ln -s ../src/ioaccess.c FreeBSD/ioaccess.c
	$(MAKE) -C FreeBSD
	-cp FreeBSD/wd_drv.ko ./bin
.endif

config:
	-rm -rf ./include/config.h
	-echo "//Created by Makefile, do not edit it" >> ./include/config.h
.if $(DIRECT_IO_ACCESS) == 1
	-echo "#define DIRECT_IO_ACCESS" >> ./include/config.h
	-echo "#undef LANNER_DRIVER" >> ./include/config.h
.else
	-echo "#undef DIRECT_IO_ACCESS" >> ./include/config.h
	-echo "#define LANNER_DRIVER" >> ./include/config.h
.endif	
	-echo "#define FreeBSD_ENV" >> ./include/config.h

clean:
	gmake -C src clean
.if $(LANNER_DRIVER) == 1
	$(MAKE) -C FreeBSD clean
.endif
	-rm -f include/config.h
	-rm -rf bin/wd_tst
	-rm -rf bin/wd_drv.ko
	-rm -rf bin/wd_drv.o
	-rm -rf FreeBSD/ioaccess.c

.PHONY: all clean config
