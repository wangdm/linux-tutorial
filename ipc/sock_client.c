//
// Created by wangdm on 8/6/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <pthread.h>

#include <signal.h>

#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCK_NAME "/tmp/message.sock"

static int g_quit = 0;

void *process_thread(void *arg) {
    int sock = (int) arg;
    pid_t pid = getpid();
    int count = 0;

    while (0 == g_quit) {
        char buf[1024];
        sprintf(buf, "Message from %d, count[%d]\n", pid, count++);
        int size = send(sock, buf, sizeof(buf), 0);
        if (size <= 0) {
            printf("send failed with [%d]%s\n", errno, strerror(errno));
            break;
        } else if (size > 0) {
            printf("Send message success\n");
        }
        sleep(1);
    }
}

void signal_handler(int signum) {
    printf("Receive signal %d\n", signum);
}

int main(int argc, char *argv[]) {
    struct sigaction sig;
    sig.sa_flags = 0;
    sig.sa_handler = signal_handler;

    sigaction(SIGSEGV, &sig, NULL);
    sigaction(SIGPIPE, &sig, NULL);

    struct sockaddr_un server_addr;
    int server_fd = socket(AF_LOCAL, SOCK_STREAM, 0);

    server_addr.sun_family = AF_LOCAL;
    strcpy(server_addr.sun_path, SOCK_NAME);

    if (connect(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) != 0) {
        printf("connect failed with [%d]%s\n", errno, strerror(errno));
        return 0;
    }

    pthread_t thread;
    if (pthread_create(&thread, NULL, process_thread, (void *) server_fd) != 0) {
        printf("create thread failed with [%d]%s\n", errno, strerror(errno));
    } else {
        pthread_join(thread, NULL);
    }

    close(server_fd);
}