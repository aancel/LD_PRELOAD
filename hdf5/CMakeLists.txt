cmake_minimum_required(VERSION 2.8)

INCLUDE(CheckIncludeFileCXX)

find_package(HDF5 REQUIRED)
find_package(MPI REQUIRED)

CHECK_INCLUDE_FILE_CXX (cxxabi.h HAVE_CXXABI_H)

if( HDF5_FOUND )
    if( HDF5_IS_PARALLEL )
        include_directories( ${HDF5_INCLUDE_DIRS} )
        include_directories( ${MPI_INCLUDE_PATH} )
        #option(OVERRIDE_FILE_OPEN 0)
        #option(OVERRIDE_FILE_WRITE_ORDERED 0)

        configure_file("${CMAKE_SOURCE_DIR}/config.h.in" "${CMAKE_SOURCE_DIR}/config.h")

        add_definitions(-fPIC -D_GNU_SOURCE)
        add_library(HDF5 SHARED hdf5.c)
        target_link_libraries(HDF5 ${HDF5_C_LIBRARIES} ${MPI_C_LIBRARIES} dl)
    endif()
endif()
