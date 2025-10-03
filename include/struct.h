#ifndef _MY_STRUCT_H
#define _MY_STRUCT_H

#include <stdio.h>
#include <netinet/in.h>
#include <pthread.h>
// Struct for client data
typedef struct client_info_t
{
    int fd;
    struct sockaddr_in addr;
    char name[255];
    pthread_t thread_id;
} client_info_t;

#endif