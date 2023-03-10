CMAKE_MINIMUM_REQUIRED(VERSION 3.1)
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
option(EXAMPLE "shall we complie example" on)
PROJECT(SmartCar LANGUAGES CXX C)

message("-- system processor: ${CMAKE_SYSTEM_PROCESSOR}")

set(CMAKE_CXX_STANDARD 11)

SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb ")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

MESSAGE(STATUS ${CMAKE_SYSTEM_NAME})

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
"${CMAKE_SOURCE_DIR}"
"${CMAKE_SOURCE_DIR}/include"
"${CMAKE_SOURCE_DIR}/drivers" # 需要SOC资源
"${CMAKE_SOURCE_DIR}/moto"    # 我需要轮子
"${CMAKE_SOURCE_DIR}/remote"  # 需要遥控器
"${CMAKE_SOURCE_DIR}/sensor"  # 需要传感器
"${CMAKE_SOURCE_DIR}/smartcar" # 这里是汽车工厂
"${CMAKE_SOURCE_DIR}/sensor/camera"
# "${CMAKE_SOURCE_DIR}/sensor/audio_source"
"${CMAKE_SOURCE_DIR}/sensor/H264_camera"
"${CMAKE_SOURCE_DIR}/live_video/ringbuf"
"${CMAKE_SOURCE_DIR}/live_video"
"${CMAKE_SOURCE_DIR}/utils"
"${CMAKE_SOURCE_DIR}/sensor/mpu6050"
"${CMAKE_SOURCE_DIR}/sensor/mpu9250"
"${LIVEINCLUDE}"
)

add_subdirectory(lib_json)
#add_subdirectory(protobuf)

add_library (liblive555 STATIC ${LIVESOURCE})

set(EXEC_NAME SmartCar)

file(GLOB main_srcs 
"${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/drivers/*.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/sensor/*.cpp"
# "${CMAKE_CURRENT_SOURCE_DIR}/sensor/audio_source/*cpp"
# "${CMAKE_CURRENT_SOURCE_DIR}/sensor/camera/*.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/sensor/H264_camera/*.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/moto/*.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/remote/*.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/smartcar/*.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/live_video/*.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/live_video/H264_camera/*.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/live_video/ringbuf/*.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/utils/*.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/sensor/mpu6050/*.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/sensor/mpu9250/*.cpp"
)

add_executable(${EXEC_NAME}
${LIVESOURCE}
${main_srcs}
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
        RUNTIME DESTINATION /usr/sbin)
