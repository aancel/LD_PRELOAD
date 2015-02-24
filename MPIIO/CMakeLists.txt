cmake_minimum_required(VERSION 2.8)

find_package(MPI REQUIRED)
include_directories(${MPI_C_INCLUDE_PATH})

option(OVERRIDE_INIT 0)
option(OVERRIDE_FILE_OPEN 0)
option(OVERRIDE_FILE_READ_ALL 0)
option(OVERRIDE_FILE_WRITE_ORDERED 0)
option(ENABLE_TIMING 0)

configure_file("${CMAKE_SOURCE_DIR}/config.h.in" "${CMAKE_SOURCE_DIR}/config.h")

add_definitions(-fPIC -D_GNU_SOURCE)
add_library(MPIIO SHARED mpiio.c)
target_link_libraries(MPIIO ${MPI_C_LIBRARIES} dl)
