CMAKE_MINIMUM_REQUIRED(VERSION 3.5)
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
option(EXAMPLE "shall we complie example" on)
project(ros_car LANGUAGES CXX C)

message("-- system processor: ${CMAKE_SYSTEM_PROCESSOR}")

# Default to C99
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb ")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

MESSAGE(STATUS ${CMAKE_SYSTEM_NAME})

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)

set(LIVE555URL http://www.live555.com/liveMedia/public/live555-latest.tar.gz CACHE STRING "live555 url")
set(LIVE555CFLAGS -DBSD=1 -DSOCKLEN_T=socklen_t -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE=1 -DALLOW_RTSP_SERVER_PORT_REUSE=1 -DNO_OPENSSL=1 CACHE STRING "live555 CFGLAGS")

# openssl ?
find_package(OpenSSL QUIET)
MESSAGE("OpenSSL_FOUND = ${OpenSSL_FOUND}")

# live555
find_package(libliveMedia)
find_package(libUsageEnvironment)
find_package(libBasicUsageEnvironment)
find_package(libgroupsock)
if (LIBLIVEMEDIA_FOUND AND LIBBASICUSAGEENVIRONMENT_FOUND AND LIBUSAGEENVIRONMENT_FOUND AND LIBGROUPSOCK_FOUND)
    message(STATUS "live555 loaded")
    set(LIVEINCLUDE ${LIBLIVEMEDIA_INCLUDE_DIR} ${LIBBASICUSAGEENVIRONMENT_INCLUDE_DIR} ${LIBUSAGEENVIRONMENT_INCLUDE_DIR} ${LIBGROUPSOCK_INCLUDE_DIR})
    SET(LIVE_LIBRARIES ${LIBLIVEMEDIA_LIBRARY} ${LIBUSAGEENVIRONMENT_LIBRARY} ${LIBBASICUSAGEENVIRONMENT_LIBRARY} ${LIBGROUPSOCK_LIBRARY}) 
    SET(CPACK_DEBIAN_PACKAGE_DEPENDS ${CPACK_DEBIAN_PACKAGE_DEPENDS}livemedia-utils,)
else()
    set(LIVE ${CMAKE_BINARY_DIR}/live)
    set(LIVEINCLUDE ${LIVE}/groupsock/include ${LIVE}/liveMedia/include ${LIVE}/UsageEnvironment/include ${LIVE}/BasicUsageEnvironment/include)
    if (NOT EXISTS ${LIVE})
        file (DOWNLOAD ${LIVE555URL} ${CMAKE_BINARY_DIR}/live555-latest.tar.gz )
        EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E tar xvf ${CMAKE_BINARY_DIR}/live555-latest.tar.gz  RESULT_VARIABLE unpack_result)
        if(NOT unpack_result STREQUAL "0")
            message(FATAL_ERROR "Fetching and compiling live555 failed!")
        endif()
    endif()
    FILE(GLOB LIVESOURCE ${LIVE}/groupsock/*.c* ${LIVE}/liveMedia/*.c* ${LIVE}/UsageEnvironment/*.c* ${LIVE}/BasicUsageEnvironment/*.c*)
    if (NOT OpenSSL_FOUND)
        set(LIVE555CFLAGS ${LIVE555CFLAGS} -DNO_OPENSSL=1)
    endif()
    add_definitions(${LIVE555CFLAGS})
endif()

include_directories(
    src/include
    src/drivers # 需要SOC资源
    src/moto    # 我需要轮子
    src/remote  # 需要遥控器
    src/sensor  # 需要传感器
    src/smartcar # 这里是汽车工厂
    src/sensor/camera
#   src/sensor/audio_source
    src/sensor/H264_camera
    src/live_video/ringbuf
    src/live_video
    src/utils
    src/sensor/mpu6050
    src/sensor/mpu9250
    ${LIVEINCLUDE}
)

add_subdirectory(src/lib_json)
#add_subdirectory(src/protobuf)

add_library (liblive555 STATIC ${LIVESOURCE})

set(EXEC_NAME ros_car)

file(GLOB main_srcs 
    src/*.cpp
    src/drivers/*.cpp
    src/sensor/*.cpp
#   src/sensor/audio_source/*cpp
#   src/sensor/camera/*.cpp
    src/sensor/H264_camera/*.cpp
    src/moto/*.cpp
    src/remote/*.cpp
    src/smartcar/*.cpp
    src/live_video/*.cpp
    src/live_video/H264_camera/*.cpp
    src/live_video/ringbuf/*.cpp
    src/utils/*.cpp
    src/sensor/mpu6050/*.cpp
    src/sensor/mpu9250/*.cpp
)

add_executable(${EXEC_NAME}
    ${LIVESOURCE}
    ${main_srcs}
)

target_include_directories(${EXEC_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)

ament_target_dependencies(
    ${EXEC_NAME}
    "rclcpp"
)

target_link_libraries(${EXEC_NAME}
    pthread
    m
    dl
    json
    # carproto
    # protobuf-lite
)

install(TARGETS ${EXEC_NAME}
    DESTINATION lib/${PROJECT_NAME})

#if(BUILD_TESTING)
#  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # uncomment the line when a copyright and license is not present in all source files
  #set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # uncomment the line when this package is not in a git repo
  #set(ament_cmake_cpplint_FOUND TRUE)
#  ament_lint_auto_find_test_dependencies()
#endif()

ament_package()