# Comment/uncomment the following line to disable/enable debugging
#DEBUG = y


# Add your debugging flag (or not) to CFLAGS
ifeq ($(DEBUG),y)
  DEBFLAGS = -O -g # "-O" is needed to expand inlines
else
  DEBFLAGS = -O2
endif

EXTRA_CFLAGS += $(DEBFLAGS) -I$(LDDINC)

ifneq ($(KERNELRELEASE),)
# call from kernel build system

pvdspb_adc-objs := pvdspb_adc_main.o

obj-m	:= pvdspb_adc.o

else

#KERNELDIR ?= /lib/modules/$(shell uname -r)/build
#KERNELDIR ?= /workspace/devel/pv_dspb3725/kernel/kernel/kernel
KERNELDIR ?= /home/hl1sqi/work/labs/kernel/kernel
PWD       := $(shell pwd)

modules:
	echo $(MAKE) -C $(KERNELDIR) M=$(PWD) LDDINC=$(PWD)/../include modules
	$(MAKE) -C $(KERNELDIR) M=$(PWD) LDDINC=$(PWD)/../include modules

endif



clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions *.order *.symvers

depend .depend dep:
	$(CC) $(CFLAGS) -M *.c > .depend


ifeq (.depend,$(wildcard .depend))
include .depend
endif
