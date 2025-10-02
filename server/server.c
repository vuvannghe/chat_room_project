#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>

#include <time.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFF_SIZE 4097   // Message toi da BUFF_SIZE-1 ki tu
#define CLIENT_LIMIT 255 // Gioi han co luong client toi da
#define SERVER_ADDR "192.168.3.104"
#define PORT 8000

int max_client = 0; // Bien luu so luong client toi da
int client_count = 0;
FILE *log_file;
const char *writefile_format = "%02d/%02d/%04d - %02d:%02d:%02d  Client:%-5d Address:%s Client name: %-6s Message:%s\n"; // Format: dd/mm/yyy - hh:mm:ss Client_fd Client_address Message

int server_fd;
typedef struct client_info_t
{
    int fd;
    struct sockaddr_in addr;
    char name[255];
} client_info_t;
client_info_t *client_list;

/*Thread*/
pthread_t accept_thread;
pthread_t *client_thread;
pthread_mutex_t client_mutex;
pthread_cond_t client_cond;

void add_client(int new_fd, struct sockaddr_in new_addr, const char *name)
{
    client_list[client_count].fd = new_fd;
    client_list[client_count].addr = new_addr;
    strcpy(client_list[client_count].name, name);
}
void *client_thread_handler(void *arg);
void *acc_thread_handler(void *arg)
{
    int new_fd = -1;
    struct sockaddr_in new_addr;
    socklen_t addr_len = sizeof(new_addr);
    int xyz;
    char name_buff[256];
    while (1)
    {
        if (client_count > max_client)
        {
            pthread_mutex_lock(&client_mutex);
            while (client_count > max_client)
            {
                pthread_cond_wait(&client_cond, &client_mutex);
            }
            pthread_mutex_unlock(&client_mutex);
        }

        if ((new_fd = accept(server_fd, (struct sockaddr *)&new_addr, &addr_len)) >= 0)
        {
            pthread_mutex_lock(&client_mutex);
            if ((xyz = recv(new_fd, name_buff, 256, 0)) > 0)
            {
                add_client(new_fd, new_addr, name_buff);
                int *i = (int *)malloc(sizeof(int));
                *i = client_count;
                if (pthread_create(&client_thread[client_count], NULL, client_thread_handler, (void *)i) == 0)
                {
                    fprintf(stdout, "New client connected:\nSocket fd:%d\tIP address: %s\tClient name: %s\n", client_list[client_count].fd, inet_ntoa(client_list[client_count].addr.sin_addr), client_list[client_count].name);
                    pthread_detach(client_thread[client_count]);
                    client_count++;
                }
                else
                {
                    client_count--;
                    fprintf(stdout, "Connection failed due to creating new thread unsuccessfully\n");
                }
            }
            else
            {
                fprintf(stdout, "Can't get client name %s - %d\n", name_buff, xyz);
                close(new_fd);
            }

            pthread_mutex_unlock(&client_mutex);
        }
        else
        {
            fprintf(stdout, "New connection failed %d\n", new_fd);
        }
    }
}

void *client_thread_handler(void *arg)
{
    int client_index = *(int *)arg;
    free(arg);
    char buffer[BUFF_SIZE];
    char send_buffer[BUFF_SIZE + 1024];
    time_t now;
    struct tm *t;
    int xyz;
    while (1)
    {
        if ((xyz = recv(client_list[client_index].fd, buffer, BUFF_SIZE, 0)) > 0)
        {
            if (strcmp(buffer, "end_cmd") != 0)
            {
                now = time(NULL);
                t = localtime(&now);
                fprintf(log_file, writefile_format, t->tm_mday, t->tm_mon + 1, t->tm_year + 1900, t->tm_hour, t->tm_min, t->tm_sec, client_list[client_index].fd, inet_ntoa(client_list[client_index].addr.sin_addr), client_list[client_index].name, buffer);
                fflush(log_file);
                sprintf(send_buffer, "%s: %s", client_list[client_index].name, buffer);
                for (int i = 0; i < client_count; i++)
                {
                    if (i != client_index)
                    {
                        send(client_list[i].fd, send_buffer, strlen(send_buffer) + 1, 0);
                        fprintf(stdout, "Sent\n");
                    }
                }
                memset(buffer, 0, strlen(buffer));
                memset(send_buffer, 0, strlen(send_buffer));
            }
            else
            {
                for (int i = 0; i < client_count; i++)
                {
                    if (i != client_index)
                    {
                        pthread_cancel(client_thread[i]);
                    }
                    pthread_cancel(accept_thread);
                    pthread_exit(NULL);
                }
            }
        }
        else
        {
            perror("Failed to receive message\n");
            // pthread_exit(NULL);
        }
    }
}

int main(int argc, char *argv[])
{
    // Set timezone
    setenv("TZ", "ICT-7", 1);
    tzset();
    if ((argc == 1) || (argc > 2))
    {
        fprintf(stdout, "Invalid number of command-line args\n");
        exit(1);
    }
    max_client = atoi(*(argv + 1));
    if ((max_client <= 1) || (max_client > CLIENT_LIMIT))
    {
        fprintf(stdout, "Invalid max client number (2 <= n <= %d)\n", CLIENT_LIMIT);
        exit(1);
    }

    /*SOCKET*/
    struct sockaddr_in server_addr;

    server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_fd < 0)
    {
        perror("Failed to create socket");
        exit(1);
    }

    server_addr.sin_port = htons(PORT);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in)) < 0)
    {
        perror("Failed to bind server address\n");
        goto end;
    }

    if (listen(server_fd, 10) < 0)
    {
        perror("Failed\n");
        goto end;
    }

    client_list = (client_info_t *)malloc(max_client * sizeof(client_info_t)); // Cap phat dong cho danh sach client
    client_thread = (pthread_t *)malloc(max_client * sizeof(pthread_t));       // Cap phat dong mang thread ID cho cac thread lang nghe client

    /*Mo file log*/
    log_file = fopen("history.log", "a");
    if (log_file == NULL)
    {
        fprintf(stdout, "Failed to open log file\n");
        free(client_list);
        free(client_thread);
        goto end;
    }

    /*Thread*/
    pthread_mutex_init(&client_mutex, NULL);
    pthread_cond_init(&client_cond, NULL);

    pthread_create(&accept_thread, NULL, acc_thread_handler, NULL);
    pthread_join(accept_thread, NULL);
end:
    close(server_fd);
    return 0;
}