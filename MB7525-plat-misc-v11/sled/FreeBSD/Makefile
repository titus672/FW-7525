#$FreeBSD$
.PATH:  ${.CURDIR}
KMOD    = sled_drv
SRCS    = device_if.h bus_if.h 
SRCS    += ../include/ioaccess.h ../include/version.h ../include/sled_ioctl.h
SRCS    += drv_main.c ioaccess.c

clean:
	rm -f device_if.h bus_if.h pci_if.h 
	rm -f *.o *.kld *.ko
	rm -f @ machine
	rm -f export_syms

.include <bsd.kmod.mk>
