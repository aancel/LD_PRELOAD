/**
   \file mpiio.cpp
   \author Alexandre Ancel (https://github.com/aancel)
   \date 2014-06-19

   Modify behavior of MPI IO functions to ensure data is correctly written.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <mpi.h>
#include <dlfcn.h>

#include "config.h"

#define _GNU_SOURCE 1

// Buffer size (not foolproof)
#define BUFSIZE 256

// Define a limit for displaying n values at most
#define PRINT_LIMIT 10

typedef struct
{
    MPI_Datatype type;
    char * name;
} TrMPITypeName;

TrMPITypeName trMPITypeName[] = 
{ 
{ MPI_PACKED, "PACKED" },
{ MPI_CHAR, "CHAR" },  
{ MPI_UNSIGNED_CHAR, "UNSIGNED CHAR" },  
{ MPI_SHORT, "SHORT" },  
{ MPI_UNSIGNED_SHORT, "UNSIGNED SHORT" },  
{ MPI_INT, "INT" },  
{ MPI_LONG, "LONG" },  
{ MPI_UNSIGNED, "UNSIGNED" },  
{ MPI_UNSIGNED_LONG, "UNSIGNED LONG" },  
{ MPI_INT32_T, "INT32" },  
{ MPI_INT64_T, "INT64" },  
{ MPI_FLOAT, "FLOAT" }, 
{ MPI_DOUBLE, "DOUBLE" }, 
{ MPI_LONG_DOUBLE, "LONG_DOUBLE" }, 
{ 0, NULL } 
}; 

/* translate MPI type name to string */
char * translateMPIType( MPI_Datatype type )
{
    int i = 0;
    while(trMPITypeName[i].name) 
    {
        if(trMPITypeName[i].type == type)
        {
            return trMPITypeName[i].name;
        }
        i++;
    }
    return NULL;
}

#if OVERRIDE_INIT
int MPI_Init(int *argc, char ***argv)
{
    static int (*f)();

    /* get the new symbol corresponding to the function with the same name */
    if(!f)
        f = (int(*)()) dlsym(RTLD_NEXT, "MPI_Init");

    printf("[MPI] Calling MPI_Init\n");

    int res = f(argc, argv);

    /* call the original MPI function */
    return res;
}
#endif

#if OVERRIDE_FINALIZE
int MPI_Finalize()
{
    static int (*f)();

    /* get the new symbol corresponding to the function with the same name */
    if(!f)
        f = (int(*)()) dlsym(RTLD_NEXT, "MPI_Finalize");

    printf("[MPI] Calling MPI_Finalize\n");

    int res = f(argc, argv);

    /* call the original MPI function */
    return res;
}
#endif

#if OVERRIDE_FILE_OPEN
int MPI_File_open(MPI_Comm comm, const char *filename, int amode, MPI_Info info, MPI_File *fh)
{
    int rank, size;
    char b1[BUFSIZE];
    char b2[BUFSIZE];
    static int (*f)();

    static unsigned int callIdx = 0;

    memset(b1, 0, BUFSIZE);
    memset(b2, 0, BUFSIZE);

    /* get the new symbol corresponding to the function with the same name */
    if(!f)
        f = (int(*)()) dlsym(RTLD_NEXT, "MPI_File_open");

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    sprintf(b1, "[MPI_File_open:id_%d] global %d/%d ", callIdx, rank, size);
    strcat(b2, b1);

    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);

    sprintf(b1, "local %d/%d %s %d", rank, size, filename, amode);
    strcat(b2, b1);

    printf("%s\n", b2);

    int res = f(comm, filename, amode, info, fh);

    callIdx++;

    /* call the original MPI function */
    return res;
}
#endif

#if OVERRIDE_FILE_CLOSE
int MPI_File_close(MPI_File * fh)
{
    int rank, size;
    char b1[BUFSIZE];
    char b2[BUFSIZE];
    static int (*f)();

    static unsigned int callIdx = 0;

    memset(b1, 0, BUFSIZE);
    memset(b2, 0, BUFSIZE);

    /* get the new symbol corresponding to the function with the same name */
    if(!f)
        f = (int(*)()) dlsym(RTLD_NEXT, "MPI_File_close");

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    sprintf(b1, "[MPI_File_close:id_%d] global %d/%d ", callIdx, rank, size);
    strcat(b2, b1);

    printf("%s\n", b2);

    int res = f(fh);

    callIdx++;

    /* call the original MPI function */
    return res;
}
#endif

#if OVERRIDE_FILE_READ_ALL
int MPI_File_read_all(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
    int i;
    char b[BUFSIZE];
    static int (*f)();

    static unsigned int callIdx = 0;

    /* get a string containing the MPI type */
    char * tname = translateMPIType(datatype);

    /* get the new symbol corresponding to the function with the same name */
    if(!f)
        f = (int(*)()) dlsym(RTLD_NEXT, "MPI_File_read_all");

    /* Gather MPI data about current rank for display */
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

#if ENABLE_TIMING
    struct timespec ts1;
    struct timespec ts2;
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts1);
#endif

    int res = f(fh, buf, count, datatype, status);

#if ENABLE_TIMING
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts2);
    double t1 = (double)(ts1.tv_sec) + (double)(ts1.tv_nsec) / (1000000000.0);                                                                                                        double t2 = (double)(ts2.tv_sec) + (double)(ts2.tv_nsec) / (1000000000.0);
    printf("[MPIIO] P%d/%d: MPI_File_read_all (lasted: %fs)\n", rank, callIdx, t2 - t1);
#endif
    
    // increment the call index
    callIdx++;

    /* call the original MPI function */
    return res;
}
#endif

#if OVERRIDE_FILE_WRITE_ORDERED
// overriden function
int MPI_File_write_ordered(MPI_File fh, const void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
    int i;
    char b[BUFSIZE];
    static int (*f)();

    static unsigned int callIdx = 0;

    /* get a string containing the MPI type */
    char * tname = translateMPIType(datatype);

    /* get the new symbol corresponding to the function with the same name */
    if(!f)
        f = (int(*)()) dlsym(RTLD_NEXT, "MPI_File_write_ordered");

    /* Gather MPI data about current rank for display */
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    /* Display info according to the MPI type given in parameter */
    if(datatype == MPI_CHAR)
    {
        char * s = strndup(buf, count);
        printf("[MPIIO] P%d/%d: [%s;%d] %s\n", rank, callIdx, tname, count, s);
        free(s);
    }
    else if(datatype == MPI_INT || datatype == MPI_INT32_T || datatype == MPI_INT64_T)
    {
        int min, max, val;
        char * s = calloc(BUFSIZE * count, sizeof(char));
        strcat(s, "{");

        min = max = ((int *)buf)[0];
        for(i = 0; i < count; i++)
        {
            val = ((int *)buf)[i];
            /* min/max */
            if(val < min) { min = val; }
            if(val > max) { max = val; }

            /* Limit printed output */
            if(i < PRINT_LIMIT)
            {
                sprintf(b, " %d ", val); 
                strcat(s, b);
            }
        }
        if(PRINT_LIMIT < count)
        {
            strcat(s, " ... ");
        }
        strcat(s, "}");
        printf("[MPIIO] P%d/%d: [%s: %d (%d, %d) ] %s\n", rank, callIdx, tname, count, min, max, s);
        free(s);
    }
    else if(datatype == MPI_FLOAT)
    {
        float min, max, val;
        char * s = calloc(BUFSIZE * count, sizeof(char));
        strcat(s, "{");
        for(i = 0; i < count; i++)
        {
            val = ((float *)buf)[i];
            /* min/max */
            if(val < min) { min = val; }
            if(val > max) { max = val; }

            /* Limit printed output */
            if(i < PRINT_LIMIT)
            {
                sprintf(b, " %f ", val); 
                strcat(s, b);
            }
        }
        if(PRINT_LIMIT < count)
        {
            strcat(s, " ... ");
        }
        strcat(s, "}");
        printf("[MPIIO] P%d/%d: [%s : %d (%f, %f) ] %s\n", rank, callIdx, tname, count, min, max, s);
        free(s);
    }
    else if(tname != NULL)
    {
        printf("[MPIIO] P%d/%d: [%s : %d ] No implementation to print this data type\n", rank, callIdx, tname, count);
    }
    else
    {
        printf("[MPIIO] P%d/%d: Unknown MPI datatype (%d)\n", rank, callIdx, datatype);
    }

    //printf("P%d: Begin Call\n", rank);
#if ENABLE_TIMING
    struct timespec ts1;
    struct timespec ts2;
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts1);
#endif

    int res = f(fh, buf, count, datatype, status);

#if ENABLE_TIMING
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts2);
    double t1 = (double)(ts1.tv_sec) + (double)(ts1.tv_nsec) / (1000000000.0);                                                                                                        double t2 = (double)(ts2.tv_sec) + (double)(ts2.tv_nsec) / (1000000000.0);
    printf("[MPIIO] P%d/%d: Call lasted: %fs\n", rank, callIdx, t2 - t1);
#endif
    
    //printf("P%d %d: End Call\n", rank, count);
    
    // increment the call index
    callIdx++;

    /* call the original MPI function */
    return res;
}
#endif
