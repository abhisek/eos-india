obj-m := rootkit.o

#rootkit-objs := immDR.o utils.o hooktable.o DR.o
rootkit-objs := syscallhook.o utils.o hooktable.o DR.o

KDIR    := /lib/modules/$(shell uname -r)/build
PWD 	:= $(shell pwd)

all:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
clean:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) clean
	$(RM) Module.markers modules.order Module.symvers a.out
