cmake_minimum_required(VERSION 2.8)

find_package(MPI REQUIRED)
include_directories(${MPI_C_INCLUDE_PATH})

add_definitions(-fPIC -D_GNU_SOURCE)
add_library(MPIIO SHARED mpiio.c)
target_link_libraries(MPIIO ${MPI_C_LIBRARIES} dl)
