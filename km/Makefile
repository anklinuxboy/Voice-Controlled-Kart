# Prepared by Team BlueGum
# Juan Jhong Chung, Laura Kamfonik, & Ankit Sharma
# EC535 Fall 2014 Final Project

# Makefile for gum_km.c

ifneq ($(KERNELRELEASE),)
	obj-m := gum_km.o
else
	KERNELDIR := $(EC535)/gumstix/oe/linux-2.6.21/
	PWD := $(shell pwd)
	ARCH := arm
	CROSS := arm-linux-

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS) modules

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) ARCH=$(ARCH) clean

endif
