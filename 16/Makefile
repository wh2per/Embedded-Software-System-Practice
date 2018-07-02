KDIR := ~/linux-rpi
ARM := ARCH=arm CROSS_COMPILE=arm-linux-gnueabi-
PWD := $(shell pwd)

obj-m := pir_tasklet.o

default:
	$(MAKE) -C $(KDIR) M=$(PWD) $(ARM) modules
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) $(ARM) clean
