//
// Created by wangdm on 8/6/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <pthread.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCK_NAME "/tmp/message.sock"

typedef struct client_session_s
{
    int sock;
    pthread_t thread;
    struct sockaddr_un address;
}client_session;

static int g_quit = 0;

void* process_thread(void* arg)
{
    client_session* session = (client_session*)arg;
    while (0 == g_quit)
    {
        char buf[1024];
        int rsize = recv(session->sock, buf, sizeof(buf), 0);
        if (0 > rsize)
        {
            printf("recv failed with [%d]%s\n", errno, strerror(errno));
            break;
        }
        else if(0 == rsize)
        {
            printf("client disconnected\n");
            break;
        }
        else if (0 < rsize)
        {
            printf("%s", buf);
        }
    }
    close(session->sock);
    free(session);
}

void* listen_thread(void* arg)
{
    struct sockaddr_un server_addr;
    struct sockaddr_un client_addr;

    if(unlink (SOCK_NAME) != 0)
    {
        printf("unlink failed with [%d]%s\n", errno, strerror(errno));
    }

    int server_fd = socket(AF_LOCAL, SOCK_STREAM, 0);

    server_addr.sun_family = AF_LOCAL;
    strcpy(server_addr.sun_path, SOCK_NAME);

    if(bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) != 0)
    {
        printf("bind failed with [%d]%s\n", errno, strerror(errno));
        return NULL;
    }

    if(listen(server_fd, 8) != 0)
    {
        printf("listen failed with [%d]%s\n", errno, strerror(errno));
        return NULL;
    }

    printf("listen client ...\n");
    while (0 == g_quit)
    {
        socklen_t addr_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
        if (0 < client_fd)
        {
            printf("client connected from %s\n", client_addr.sun_path);
            client_session* session = malloc(sizeof(client_session));
            if(NULL == session)
            {
                continue;
            }
            memset(session, 0, sizeof(client_session));
            session->sock = client_fd;
            memcpy(&session->address, &client_addr, sizeof(client_addr));

            if(pthread_create(&session->thread, NULL, process_thread, session) != 0)
            {
                printf("create thread failed with [%d]%s\n", errno, strerror(errno));
            }
        }
        else
        {
            printf("accept failed with [%d]%s\n", errno, strerror(errno));
        }
    }

    close(server_fd);
}

int main(int argc, char* argv[])
{
    pthread_t t_listen;
    if(pthread_create(&t_listen, NULL, listen_thread, NULL) != 0)
    {
        printf("create listen thread failed with [%d]%s\n", errno, strerror(errno));
        return 0;
    }
    pthread_join(t_listen, NULL);
}