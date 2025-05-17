obj-m := hello.o
KERNEL_SOURCE = /
all :
	make -C $(KERNEL_SOURCE) M=$(PWD) modules
clean :
	make -C $(KERNEL_SOURCE) M=$(PWD) clean
install :
	make -c $(KERNEL_SOURCE) M=$(PWD) install
