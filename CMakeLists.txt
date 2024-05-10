cmake_minimum_required(VERSION 3.10)
project(ExpenseTracker VERSION 0.0.1 LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_USE_RESPONSE_FILE_FOR_INCLUDES OFF)
set(CMAKE_CXX_USE_RESPONSE_FILE_FOR_INCLUDES OFF)
if(WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
add_definitions(-DUNICODE)
add_definitions(-D_UNICODE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unknown-pragmas")

set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}")

find_package(OpenSSL REQUIRED)
if(NOT OPENSSL_FOUND)
    message(FATAL "Cannot find OpenSSL, please check environment")
endif()
include(cmake/copy_openssl_dlib.cmake)

find_package(Git REQUIRED)
execute_process(
    COMMAND ${GIT_EXECUTABLE} log -1 --pretty=format:%h
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
execute_process(
    COMMAND ${GIT_EXECUTABLE} symbolic-ref --short -q HEAD
    OUTPUT_VARIABLE GIT_BRANCH
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
string(TIMESTAMP BUILD_TIME %Y.%m.%d)
configure_file(
    ${CMAKE_SOURCE_DIR}/build_cfgs.h.in
    ${CMAKE_SOURCE_DIR}/core/include/core/build_cfgs.h
    @ONLY
)
# parse build cfgs

include(cmake/download_builtin_curl.cmake)

add_subdirectory(core)
add_subdirectory(etscript)
add_subdirectory(cli)
