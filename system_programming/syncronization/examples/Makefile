obj-m += atomic_example.o
obj-m += spinlock_example.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD  := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
#
#for user space program atomic_exmple2.c "gcc -pthread atomic_example2.c -o atomic_userspace"
