######## cross compile env define ###################
SET(CMAKE_SYSTEM_NAME Linux)
# 配置库的安装路径
SET(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install)

SET(CMAKE_SYSTEM_PROCESSOR riscv64)
# 工具链地址
SET(TOOLCHAIN_DIR  "/home/prince/Tina-Linux/lichee/brandy-2.0/tools/toolchain/riscv64-linux-x86_64-20200528/bin/")

# sunxi D1
SET(CMAKE_C_COMPILER ${TOOLCHAIN_DIR}riscv64-unknown-linux-gnu-gcc)
SET(CMAKE_CXX_COMPILER ${TOOLCHAIN_DIR}riscv64-unknown-linux-gnu-g++)
