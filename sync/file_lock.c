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

static void process_server() {

    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_HOST);
    server_addr.sin_port = htons(SERVER_PORT);

    if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) != 0) {
        LOG_ERROR("server bind failed with [%d]%s", errno, strerror(errno));
        close(server_fd);
        return;
    }

    if (listen(server_fd, 8) != 0) {
        LOG_ERROR("server listen failed with [%d]%s", errno, strerror(errno));
        close(server_fd);
        return;
    }
    LOG_INFO("server listen on %s:%d ...", SERVER_HOST, SERVER_PORT);

    for (;;) {
        if (g_app_quit) {
            break;
        }

        socklen_t addr_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr *) &client_addr, &addr_len);
        if (0 < client_fd) {
            LOG_INFO("client connected from %s", inet_ntoa(client_addr.sin_addr));
            close(client_fd);
        } else {
            LOG_ERROR("server accept failed with [%d]%s", errno, strerror(errno));
        }

        msleep(30);
    }

    close(server_fd);
}

static void process_client() {

    /// wait server
    sleep(1);

    int sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_HOST);
    server_addr.sin_port = htons(SERVER_PORT);

    if (connect(sock, (struct sockaddr *) &server_addr, sizeof(server_addr)) != 0) {
        LOG_ERROR("connect failed with [%d]%s", errno, strerror(errno));
        close(sock);
        return;
    }

    for (;;) {
        if (g_app_quit) {
            break;
        }
        msleep(30);
    }
    close(sock);
}

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

    pid_t pid = fork();
    if (pid == -1) {
        LOG_FATAL("linux fork process failed with [%d]%s", errno, strerror(errno));
        return -1;
    } else if (pid == 0) {
        /// father process
        process_server();
    } else {
        /// child process
        process_client();
    }
    return 0;
}