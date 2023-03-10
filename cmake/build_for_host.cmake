######## cross compile env define ###################
SET(CMAKE_SYSTEM_NAME Linux)
# 配置库的安装路径
SET(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install)

# SET(CMAKE_SYSTEM_PROCESSOR x86_64)

SET(PROTOBUF_INCLUDE_DIRS /usr/local/include)

# x86_64
SET(CMAKE_C_COMPILER gcc)
SET(CMAKE_CXX_COMPILER g++)