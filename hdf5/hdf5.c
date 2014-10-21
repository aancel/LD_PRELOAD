/**
   \file hdf5.cpp
   \author Alexandre Ancel (https://github.com/aancel)
   \date 2014-10-21

   Modify behavior of MPI IO functions to ensure data is correctly written.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <dlfcn.h>

#include <hdf5.h>

#include "config.h"
//#include "../include/stacktrace.h"

#define _GNU_SOURCE 1

// Buffer size (not foolproof)
#define BUFSIZE 256

// Define a limit for displaying n values at most
#define PRINT_LIMIT 10

hid_t H5Fcreate( const char *name, unsigned flags, hid_t fcpl_id, hid_t fapl_id ) 
{
    int rank, size;
    static hid_t (*f)();

    char b1[BUFSIZE];
    char b2[BUFSIZE];

    memset(b1, 0, BUFSIZE);
    memset(b2, 0, BUFSIZE);

    //H5Eset_auto(H5E_DEFAULT, NULL, NULL);

    /* get the new symbol corresponding to the function with the same name */
    if(!f)
        f = (hid_t(*)()) dlsym(RTLD_NEXT, "H5Fcreate");

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    sprintf(b1, "%d/%d Creating file %s", rank, size, name);

    printf("%s\n", b1);

    hid_t res = f(name, flags, fcpl_id, fapl_id);

    /* call the original MPI function */
    return res;
}

#if 1
hid_t H5Gcreate( hid_t loc_id, const char *name, hid_t lcpl_id, hid_t gcpl_id, hid_t gapl_id )
{
    int rank, size;
    static int (*f)();

    char b1[BUFSIZE];
    char b2[BUFSIZE];

    memset(b1, 0, BUFSIZE);
    memset(b2, 0, BUFSIZE);

    /* get the new symbol corresponding to the function with the same name */
    if(!f)
        f = (int(*)()) dlsym(RTLD_NEXT, "H5Gcreate2");

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    sprintf(b1, "%d/%d Creating group %s", rank, size, name);

    printf("%s\n", b1);

    hid_t res = f(loc_id, name, lcpl_id, gcpl_id, gapl_id);

    /* call the original MPI function */
    return res;
}

hid_t H5Dcreate( hid_t loc_id, const char *name, hid_t dtype_id, hid_t space_id, hid_t lcpl_id, hid_t dcpl_id, hid_t dapl_id ) 
{
    int rank, size;
    static int (*f)();

    char b1[BUFSIZE];
    char b2[BUFSIZE];

    memset(b1, 0, BUFSIZE);
    memset(b2, 0, BUFSIZE);

    /* get the new symbol corresponding to the function with the same name */
    if(!f)
        f = (int(*)()) dlsym(RTLD_NEXT, "H5Dcreate2");

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    sprintf(b1, "%d/%d Creating Dataset %s", rank, size, name);

    printf("%s\n", b1);

    hid_t res = f(loc_id, name, dtype_id, space_id, lcpl_id, dcpl_id, dapl_id);

    /* call the original MPI function */
    return res;
}
#endif

herr_t H5Sselect_hyperslab(hid_t space_id, H5S_seloper_t op, const hsize_t *start, const hsize_t *stride, const hsize_t *count, const hsize_t *block ) 
{
    int rank, size;
    static int (*f)();

    char b1[BUFSIZE];
    char b2[BUFSIZE];

    memset(b1, 0, BUFSIZE);
    memset(b2, 0, BUFSIZE);

    /* get the new symbol corresponding to the function with the same name */
    if(!f)
        f = (int(*)()) dlsym(RTLD_NEXT, "H5Sselect_hyperslab");

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    sprintf(b1, "%d/%d Selecting hyperslab", rank, size);

    printf("%s\n", b1);

    herr_t res = f(space_id, op, start, stride, count, block);

    /* call the original MPI function */
    return res;
}

herr_t H5Dwrite( hid_t dataset_id, hid_t mem_type_id, hid_t mem_space_id, hid_t file_space_id, hid_t xfer_plist_id, const void * buf ) 
{
    int rank, size;
    static int (*f)();

    char b1[BUFSIZE];
    char b2[BUFSIZE];

    memset(b1, 0, BUFSIZE);
    memset(b2, 0, BUFSIZE);

    /* get the new symbol corresponding to the function with the same name */
    if(!f)
        f = (int(*)()) dlsym(RTLD_NEXT, "H5Dwrite");

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    sprintf(b1, "%d/%d Writing data", rank, size);

    printf("%s\n", b1);

    herr_t res = f( dataset_id, mem_type_id, mem_space_id, file_space_id, xfer_plist_id, buf);

    sprintf(b1, "%d/%d data written", rank, size);

    /* call the original MPI function */
    return res;
}
