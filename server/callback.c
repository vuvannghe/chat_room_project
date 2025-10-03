#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>
#include "struct.h"
#include <pthread.h>

int32_t del_cb (void *data){
    //client_info_t t1 = *(client_info_t *) data;
    //pthread_cancel(t1.thread_id);
    free(data);
    return 1;
}

/**
 * @brief Compare 2 client file descriptor
 * 
 * @param data1 
 * @param data2 
 * @return 1 if data1 > data2; -1 if data1 < data2; 0 if equal
 */
int32_t cmp_cb (void *data1, void *data2){
    client_info_t t1 = *(client_info_t *) data1;
    client_info_t t2 = *(client_info_t *) data2;
    if(t1.fd > t2.fd)
        return 1;
    if(t1.fd < t2.fd)
        return -1;
    return 0; 
}