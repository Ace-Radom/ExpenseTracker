set(CURL_ZLIB OFF CACHE STRING "" FORCE)
set(CURL_USE_LIBPSL OFF CACHE STRING "" FORCE)
set(CURL_USE_LIBSSH2 OFF CACHE STRING "" FORCE)
set(CURL_USE_LIBSSH OFF CACHE STRING "" FORCE)
set(CURL_ENABLE_SSL ON CACHE INTERNAL "" FORCE)
set(CURL_USE_OPENSSL ON CACHE STRING "" FORCE)
set(CURL_ENABLE_EXPORT_TARGET OFF CACHE STRING "" FORCE)
set(CURL_DISABLE_INSTALL ON CACHE STRING "" FORCE)
# force disable curl install function
set(BUILD_CURL_EXE OFF CACHE INTERNAL "" FORCE)
set(BUILD_TESTING OFF)
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
    cmake_policy(SET CMP0010 NEW)
    cmake_policy(SET CMP0135 NEW)
endif()
include(FetchContent)
FetchContent_Declare(curl
    URL https://github.com/curl/curl/releases/download/curl-8_5_0/curl-8.5.0.tar.xz
    URL_HASH SHA256=42ab8db9e20d8290a3b633e7fbb3cec15db34df65fd1015ef8ac1e4723750eeb
    USES_TERMINAL_DOWNLOAD TRUE
)
FetchContent_MakeAvailable(curl)
# download and unpack curl 8.5.0 src package
