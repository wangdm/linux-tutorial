/**
 * Created by wangdm on 22-9-24.
**/

#include <string.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "log.h"

#define msleep(ms) usleep((ms)*1000)

#define SERVER_HOST "0.0.0.0"
#define SERVER_PORT 5566

static int g_app_quit = 0;

static void handle_signal(int signum) {
    g_app_quit = 1;
    LOG_WARN("receive signal %d", signum);
}

static void register_signal() {
    struct sigaction sa;
    memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_handler = handle_signal;
    sa.sa_flags = 0;

    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGABRT, &sa, NULL);
    //sigaction(SIGSEGV, &sa, NULL);

    sa.sa_handler = SIG_IGN;
    sigaction(SIGHUP, &sa, NULL);
    sigaction(SIGBUS, &sa, NULL);
    sigaction(SIGPIPE, &sa, NULL);
}

int main() {
    register_signal();


    int sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_HOST);
    server_addr.sin_port = htons(SERVER_PORT);

    if (connect(sock, (struct sockaddr *) &server_addr, sizeof(server_addr)) != 0) {
        LOG_ERROR("connect failed with [%d]%s", errno, strerror(errno));
        close(sock);
        return -1;
    }

    for (;;) {
        if (g_app_quit) {
            break;
        }
        char buffer[] = "Hello, TCP Server";
        ssize_t wsize = send(sock, buffer, strlen(buffer), 0);
        if (wsize < 0) {
            LOG_ERROR("client send failed with [%d]%s", errno, strerror(errno));
            break;
        }
        sleep(1);
    }

    close(sock);
    return 0;
}