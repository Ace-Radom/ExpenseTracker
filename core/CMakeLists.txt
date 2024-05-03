project(ExpenseTracker_core)

add_subdirectory(third_party)
# compile third_party libraries

set(CORE_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include" CACHE INTERNAL "${CMAKE_CURRENT_SOURCE_DIR}/include")

add_library(etcore SHARED
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/config.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/exception.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/g3log_sink.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sql_callbacks.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sql.cpp
)
target_include_directories(etcore
    PUBLIC
        ${CORE_INCLUDE_DIR}
        ${MINI_INCLUDE_DIR}
        ${G3LOG_INCLUDE_SRC_DIR}
        ${G3LOG_INCLUDE_BIN_DIR}
        ${OPENSSL_INCLUDE_DIR}
        ${SQLITE3_INCLUDE_DIR}
)
target_link_libraries(etcore
    g3log
    sqlite3
    OpenSSL::Crypto
)
