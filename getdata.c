#include <pthread.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "mergesort.h"

static int *get_list(char *arg, int *size);
static int *get_list_from_file(char *filename, int *size);
int get_token(int fd, int *notdone);

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
    *size = 0;
    char helpopt = 0, fileopt = 0, listopt = 0, threadopt = 0, threads = 0;
    char *filename;
    while((opt = getopt(argc, argv, "hf:j:l:")) != -1)
        switch(opt)
        {
            case 'f':
                printf("file option %s\n", optarg);
                fileopt = 1;
                filename = optarg;
                list = get_list_from_file(filename, size);
                break;

            case 'j':
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
                puts("-l x,y,z \t\t: enter list with elements x,y,z\n-f filename \t\t: read list from csv file\n-j x \t\t\t: make process using x amount of threads\n");
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

    if(*size > 1)
    {
        mergesort(list, *size, threads);
    }

    printf("%d", list[0]);

    int i;
    for(i = 1; i < *size; i++)
    {
        printf(",%d", list[i]);
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
            fprintf(stderr, "Integers in list are too large, no more than 12 characters per integer can be used.\n");
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

static int *get_list_from_file(char *filename, int *size) //return value must be freed
{
    int fd = open(filename, O_RDONLY);

    if(fd < 1)
    {
        fprintf(stderr, "Error opening file\n");
        exit(1);
    }

    int *out = malloc(sizeof(int));
    int notdone = 0;

    if(!out)
    {
        fprintf(stderr, "Error allocating memory\n");
        exit(1);
    }

    out[*size] = get_token(fd, &notdone);

    while(notdone)
    {
        *size += 1;

        out = realloc(out, (*size + 1) * sizeof(int));
        
        if(!out)
        {
            fprintf(stderr, "Error allocating memory");
            exit(1);
        }

        out[*size] = get_token(fd, &notdone);
    }
    
    close(fd);

    return out;
}

int get_token(int fd, int *notdone)
{
    char buf[12];
    int i = 0;
    memset(buf, '\0', 12);

    while((*notdone = read(fd, buf + i, 1)) != 0 && buf[i++] != ',')
    {
        if(*notdone < 0)
        {
            fprintf(stderr, "An error occured while reading from the file\n");
            exit(1);
        }

        if(i == 12)
        {
            fprintf(stderr, "An integer in the file was too large\n");
            exit(1);
        }
    }

    buf[i - 1] = '\0';
    return atoi(buf);
}