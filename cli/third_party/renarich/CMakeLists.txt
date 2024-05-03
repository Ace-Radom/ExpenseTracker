project(renarich LANGUAGES CXX)

set(RICH_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include" CACHE INTERNAL "${CMAKE_CURRENT_SOURCE_DIR}/include")

add_library(renarich SHARED
    ${CMAKE_CURRENT_SOURCE_DIR}/src/rich.cpp
)
target_include_directories(renarich
    PUBLIC
        ${RICH_INCLUDE_DIR}
)
