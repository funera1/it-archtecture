l-objs := led.o
obj-m := l.o

all:
	make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- \
		-C ./linux M=`pwd` modules

clean:
	make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- \
		-C ./linux M=`pwd` clean
