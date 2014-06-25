/**
   \file mpiio.cpp
   \author Alexandre Ancel (https://github.com/aancel)
   \date 2014-06-19

   Modify behavior of MPI IO functions to ensure data is correctly written.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <dlfcn.h>

#define _GNU_SOURCE 1

// Buffer size (not foolproof)
#define BUFSIZE 256

// Define a limit for displaying n values at most
#define PRINT_LIMIT 10

// overriden function
int MPI_File_write_ordered(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
{
    int i;
    char b[BUFSIZE];
    static int (*f)();

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
        printf("P%d: [char *] %s\n", rank, s);
        free(s);
    }
    else if(datatype == MPI_INT)
    {
        char * s = calloc(BUFSIZE * count, sizeof(char));
        strcat(s, "{");
        for(i = 0; i < count && i < PRINT_LIMIT; i++)
        {
            sprintf(b, " %d ", ((int *)buf)[i]); 
            strcat(s, b);
        }
        if(i < count)
        {
            strcat(s, " ... ");
        }
        strcat(s, "}");
        printf("P%d: [int * : %d] %s\n", rank, count, s);
        free(s);
    }
    else if(datatype == MPI_FLOAT)
    {
        char * s = calloc(BUFSIZE * count, sizeof(char));
        strcat(s, "{");
        for(i = 0; i < count && i < PRINT_LIMIT; i++)
        {
            sprintf(b, " %f ", ((float *)buf)[i]); 
            strcat(s, b);
        }
        if(i < count)
        {
            strcat(s, " ... ");
        }
        strcat(s, "}");
        printf("P%d: [float * : %d] %s\n", rank, count, s);
        free(s);
    }
    else
    {
        printf("Unknown MPI datatype\n");
    }

    /* call the original MPI function */
    return (f(fh, buf, count, datatype, status));
}
