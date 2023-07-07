make \
    ARCH=arm64 \
    CROSS_COMPILE=aarch64-linux-gnu- \
    -C ../linux \
    M=`pwd` \
    modules

