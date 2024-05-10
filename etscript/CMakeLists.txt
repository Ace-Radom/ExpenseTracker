project(ExpenseTracker_etscript)

set(ETSCRIPT_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include" CACHE INTERNAL "${CMAKE_CURRENT_SOURCE_DIR}/include")

add_library(etscript SHARED
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lex.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vm.cpp
)
target_include_directories(etscript
    PUBLIC
        ${ETSCRIPT_INCLUDE_DIR}
        ${CORE_INCLUDE_DIR}
    PRIVATE
        ${G3LOG_INCLUDE_BIN_DIR}
        ${G3LOG_INCLUDE_SRC_DIR}
)
target_link_libraries(etscript
    etcore
)

add_subdirectory(tests)
