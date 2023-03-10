if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.1.2)
    #-Werror=* was introduced -after- GCC 4.1.2
    add_compile_options("-Werror=strict-aliasing")
endif()

if(NOT (HAVE_CLOCALE AND HAVE_LCONV_SIZE AND HAVE_DECIMAL_POINT AND HAVE_LOCALECONV))
    message(WARNING "Locale functionality is not supported")
    if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.12.0)
        add_compile_definitions(JSONCPP_NO_LOCALE_SUPPORT)
    else()
        add_definitions(-DJSONCPP_NO_LOCALE_SUPPORT)
    endif()
endif()

set(JSONCPP_INCLUDE_DIR ../include)

set(PUBLIC_HEADERS
    ${JSONCPP_INCLUDE_DIR}/json/config.h
    ${JSONCPP_INCLUDE_DIR}/json/forwards.h
    ${JSONCPP_INCLUDE_DIR}/json/json_features.h
    ${JSONCPP_INCLUDE_DIR}/json/value.h
    ${JSONCPP_INCLUDE_DIR}/json/reader.h
    ${JSONCPP_INCLUDE_DIR}/json/version.h
    ${JSONCPP_INCLUDE_DIR}/json/writer.h
    ${JSONCPP_INCLUDE_DIR}/json/assertions.h
)

source_group("Public API" FILES ${PUBLIC_HEADERS})

set(JSONCPP_SOURCES
    json_tool.h
    json_reader.cpp
    json_valueiterator.inl
    json_value.cpp
    json_writer.cpp
)

add_library(json STATIC ${PUBLIC_HEADERS} ${JSONCPP_SOURCES})

target_include_directories(json
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})