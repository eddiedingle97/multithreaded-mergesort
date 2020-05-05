#include <pthread.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "mergesort.h"

static int *get_list(char *arg, int *size);

int main(int argc, char **argv)
{
    int opt;
    int *list;
    int *size = malloc(sizeof(int));
    if(!size)
    {
        fprintf(stderr, "Error allocating memory\n");
        return 1;
    }
    char helpopt = 0, fileopt = 0, listopt = 0, threadopt = 0, threads = 0;
    char *filename;
    while((opt = getopt(argc, argv, "hf:t:l:")) != -1)
        switch(opt)
        {
            case 'f':
                printf("file option %s\n", optarg);
                fileopt = 1;
                filename = optarg;
                break;

            case 't':
                printf("thread option %s\n", optarg);
                threadopt = 1;
                threads = atoi(optarg);
                break;

            case 'l':
                printf("list option %s\n", optarg);
                listopt = 1;
                list = get_list(optarg, size);
                break;
            
            case 'h':
                puts("-l x,y,z \t\t: enter list with elements x,y,z\n-f filename \t\t: read list from csv file\n-t x \t\t\t: make process using x amount of threads\n");
                helpopt = 1;
                break;
        }

    if(helpopt)
    {
        return 0;
    }

    if(!fileopt && !listopt)
    {
        fprintf(stdout, "A file or list argument must be given\n");
        return 0;
    }

    if(listopt)
    {
        mergesort(list, *size, threads);
    }

    int i;
    for(i = 0; i < *size; i++)
    {
        printf("%d,", list[i]);
    }
    printf("\n");

    free(list);
    free(size);

    return 0;
}

static int *get_list(char *arg, int *size) //return value must be freed
{
    int *out = malloc(sizeof(int));
    int ptr = 0, lastptr = 0, i = 0, outptr = 0;
    int done = 0;
    char *buf = malloc(16);

    if(!out || !buf)
    {
        fprintf(stderr, "Error allocating memory\n");
        exit(1);
    }

    memset(buf, '\0', 16);

    while(!done)
    {
        while(arg[ptr] != ',' && arg[ptr] != '\0')
        {
            if(arg[ptr] < '0' || arg[ptr] > '9')
            {
                fprintf(stderr, "Error parsing list\n");
                exit(1);
            }
            ptr++;
        }

        if(arg[ptr] == '\0')
            done = 1;

        if(ptr == lastptr)
            break;

        if(ptr != lastptr && arg[ptr - 1] == ',')
            break;

        if(ptr - lastptr > 12)
        {
            fprintf(stderr, "Integers in list are too large, no more than 12 digits per element can be used.\n");
            exit(1);
        }

        for(; lastptr < ptr; lastptr++)
        {
            buf[i++] = arg[lastptr];
        }
        buf[i] = '\0';
        lastptr++;
        ptr++;
        i = 0;
        
        out[outptr++] = atoi(buf);
        
        if(arg[ptr] != '\0')
        {   
            out = realloc(out, (outptr + 1) * sizeof(int));
            
            if(!out)
            {
                fprintf(stderr, "Error allocating memory");
                exit(1);
            }
        }
    }
    free(buf);

    *size = outptr;

    return out;
}