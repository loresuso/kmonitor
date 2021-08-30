ifneq ($(KERNELRELEASE),) 
obj-m := kmonitor.o
else 

KERNELDIR := /lib/modules/$(shell uname -r)/build M=$(PWD) modules

PWD := $(shell pwd)

default: 
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules  
clean: 
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
endif 
