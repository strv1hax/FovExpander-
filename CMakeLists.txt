cmake_minimum_required(VERSION 3.22.1)
project(fov-expander)
set(CMAKE_CXX_STANDARD 20)

add_library(preloader SHARED IMPORTED)
set_target_properties(preloader PROPERTIES
    IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/libs/libpreloader.so)

add_library(fov-expander SHARED
    src/fov.cpp
)

target_include_directories(fov-expander PRIVATE
    include
)

target_link_options(fov-expander PRIVATE
    -Wl,--strip-debug
    -Wl,--gc-sections
    -Wl,--exclude-libs,ALL
)

target_link_libraries(fov-expander PRIVATE
    android
    log
    preloader
)