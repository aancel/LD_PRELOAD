cmake_minimum_required(VERSION 2.8)

find_package(MPI REQUIRED)
include_directories(${MPI_C_INCLUDE_PATH})

option(OVERRIDE_ALL 0)

option(OVERRIDE_INIT 1)
option(OVERRIDE_FINALIZE 1)
option(OVERRIDE_FILE_OPEN 0)
option(OVERRIDE_FILE_CLOSE 0)
option(OVERRIDE_FILE_READ_ALL 0)
option(OVERRIDE_FILE_WRITE_ORDERED 0)
option(ENABLE_TIMING 0)

if(OVERRIDE_ALL)
    set(OVERRIDE_INIT 1)
    set(OVERRIDE_FINALIZE 1)
    set(OVERRIDE_FILE_OPEN 1)
    set(OVERRIDE_FILE_CLOSE 1)
    set(OVERRIDE_FILE_READ_ALL 1)
    set(OVERRIDE_FILE_WRITE_ORDERED 1)
    set(ENABLE_TIMING 1)
endif()

configure_file("${CMAKE_SOURCE_DIR}/config.h.in" "${CMAKE_SOURCE_DIR}/config.h")

add_definitions(-fPIC -D_GNU_SOURCE)
add_library(MPIIO SHARED mpiio.c)
target_link_libraries(MPIIO ${MPI_C_LIBRARIES} dl)
