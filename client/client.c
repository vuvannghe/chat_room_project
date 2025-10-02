#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>

#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8000
#define BUFF_SIZE 4096
pthread_t recv_thread;
pthread_t send_thread;

int client_fd;
char clientname[256];

void *thread_receive(void *arg);
void *thread_send(void *arg);

void *thread_receive(void *arg)
{
    char receive_buffer[BUFF_SIZE + 1024];
    int xyz;
    while (1)
    {
        if ((xyz = recv(client_fd, receive_buffer, BUFF_SIZE + 1024, 0)) > 0)
        {
            fprintf(stdout, "%s\n", receive_buffer);
            fflush(stdout);
            memset(receive_buffer, 0, strlen(receive_buffer));
        }
        else
        {
            if (xyz == 0)
            {
                fprintf(stdout, "Chat end\n");
                pthread_cancel(send_thread);
                pthread_exit(NULL);
            }
            else
            {
                fprintf(stdout, "Failed to receive messages\n");
            }
        }
    }
}

void *thread_send(void *arg)
{
    char send_buffer[BUFF_SIZE + 1024];
    int xyz;
    while (1)
    {
        if (fgets(send_buffer, BUFF_SIZE, stdin) != NULL)
        {
            *(send_buffer + strlen(send_buffer) - 1) = '\0';
            if (send(client_fd, send_buffer, strlen(send_buffer) + 1, 0) <= 0)
            {
                fprintf(stdout, "Failed to send message\n");
            }
        }
    }
}

int main(int argc, char *argv[]) // Doi so thu nhat la ten chuong trinh, doi so thu 2 la dia chi ipserver, doi so thu 3 la ten cua client
{
    if ((argc == 1) || (argc > 3))
    {
        fprintf(stdout, "Invalid number of argument\n");
        exit(1);
    }

    strcpy(clientname, *(argv + 2));
    fprintf(stdout, "Client name: %s\n", clientname);

    struct sockaddr_in server_address;
    client_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client_fd < 0)
    {
        perror("Failed to create socket\n");
        exit(1);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = inet_addr(*(argv + 1));
    // Connect to server socket
    if (connect(client_fd, (struct sockaddr *)&server_address, sizeof(server_address)) == 0)
    {
        if (send(client_fd, clientname, strlen(clientname) + 1, 0) == (strlen(clientname) + 1))
        {
            pthread_create(&send_thread, NULL, thread_send, NULL);
            pthread_create(&recv_thread, NULL, thread_receive, NULL);
            pthread_join(recv_thread, NULL);
            pthread_join(send_thread, NULL);
        }
        else
        {
            fprintf(stdout, "Failed to send client name to server\n");
            goto end;
        }
    }
    else
    {
        fprintf(stdout, "Failed to connect to server\n");
        goto end;
    }
end:
    close(client_fd);
}