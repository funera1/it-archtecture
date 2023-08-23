led-objs := module.o
obj-m := led.o

all:
	make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- \
		-C ../linux M=`pwd` modules

clean:
	make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- \
		-C ../linux M=`pwd` clean
