######## cross compile env define ###################
SET(CMAKE_SYSTEM_NAME Linux)
# 配置库的安装路径
SET(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install)

SET(CMAKE_SYSTEM_PROCESSOR arm)
# 工具链地址
SET(TOOLCHAIN_DIR  "/home/prince/xos/output/host/bin/")

# imx8mmini
SET(CMAKE_C_COMPILER ${TOOLCHAIN_DIR}arm-f1c100s-linux-gnueabi-gcc)
SET(CMAKE_CXX_COMPILER ${TOOLCHAIN_DIR}arm-f1c100s-linux-gnueabi-g++)
