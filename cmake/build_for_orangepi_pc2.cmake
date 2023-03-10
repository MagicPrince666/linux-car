######## cross compile env define ###################
SET(CMAKE_SYSTEM_NAME Linux)
# 配置库的安装路径
SET(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install)

# export STAGING_DIR=/Volumes/unix/HandsomeMod/staging_dir
SET(CMAKE_SYSTEM_PROCESSOR aarch64)

# 工具链地址
SET(TOOLCHAIN_DIR  "/Volumes/unix/HandsomeMod/staging_dir/toolchain-aarch64_cortex-a53_gcc-11.2.0_musl/bin/")

# xunlong orangepi pc2
SET(CMAKE_C_COMPILER ${TOOLCHAIN_DIR}aarch64-openwrt-linux-musl-gcc)
SET(CMAKE_CXX_COMPILER ${TOOLCHAIN_DIR}aarch64-openwrt-linux-musl-g++)
