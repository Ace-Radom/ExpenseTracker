project(ExpenseTracker_core)

add_subdirectory(third_party)
# compile third_party libraries

add_library(etcore SHARED
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/config.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/exception.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sql_callbacks.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sql.cpp
)
target_include_directories(etcore
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${MINI_INCLUDE_DIR}
        ${SQLITE3_INCLUDE_DIR}        
)
target_link_libraries(etcore
    sqlite3
)
