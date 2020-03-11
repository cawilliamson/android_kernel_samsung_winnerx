#!/usr/bin/env bash

mkdir -p out

ANDROID_VERSION="9.0.0"
ARCH="arm64"
export ANDROID_VERSION ARCH

BUILD_CROSS_COMPILE="/opt/aarch64-linux-android-4.9/bin/aarch64-linux-android-"
KERNEL_LLVM_BIN="/opt/llvm-arm-toolchain-ship/6.0-cfp/bin/clang"
CLANG_TRIPLE="aarch64-linux-gnu-"
KERNEL_MAKE_ENV="DTC_EXT=$(pwd)/tools/dtc CONFIG_BUILD_ARM64_DT_OVERLAY=y"

make -C $(pwd) O=$(pwd)/out $KERNEL_MAKE_ENV CFP_CC=$KERNEL_LLVM_BIN CROSS_COMPILE=$BUILD_CROSS_COMPILE REAL_CC=$KERNEL_LLVM_BIN CLANG_TRIPLE=$CLANG_TRIPLE winnerx_eur_open_defconfig
make -j$(nproc --all) -C $(pwd) O=$(pwd)/out $KERNEL_MAKE_ENV CFP_CC=$KERNEL_LLVM_BIN CROSS_COMPILE=$BUILD_CROSS_COMPILE REAL_CC=$KERNEL_LLVM_BIN CLANG_TRIPLE=$CLANG_TRIPLE

cp -v out/arch/arm64/boot/Image-dtb $(pwd)/arch/arm64/boot/Image-dtb

