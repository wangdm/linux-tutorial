/**
 * Created by wangdm on 22-9-24.
**/

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <openssl/ssl.h>
#include <openssl/err.h>

#include "log.h"

#define msleep(ms) usleep((ms)*1000)

#define TLS_CERT_PATH PROJECT_SOURCE_ROOT"/data/"
#define TLS_CERT_FILE TLS_CERT_PATH"cert.pem"
#define TLS_KEY_FILE TLS_CERT_PATH"key.pem"

#define SERVER_HOST "0.0.0.0"
#define SERVER_PORT 5566
#define BUFFER_SIZE 1024

typedef struct wdm_session_s {
    int sock;
    pthread_t tid;

    SSL_CTX *ctx;
    SSL *ssl;
} wdm_session_t;

static int g_app_quit = 0;

static void *session_process_thread(void *arg) {
    wdm_session_t *session = (wdm_session_t *) arg;

    session->ssl = SSL_new(session->ctx);
    SSL_set_fd(session->ssl, session->sock);
    if (SSL_accept(session->ssl) == -1) {
        goto QUIT;
    }

    for (;;) {
        if (g_app_quit) {
            break;
        }
        char buffer[BUFFER_SIZE];
        int rsize = SSL_read(session->ssl, buffer, BUFFER_SIZE);
        if (rsize == -1) {
            LOG_ERROR("session recv failed with [%d]%s", errno, strerror(errno));
            break;
        } else if (rsize == 0) {
            LOG_WARN("session closed");
            break;
        } else {
            LOG_INFO("receive data: %s", buffer);
        }
    }

    SSL_shutdown(session->ssl);

    QUIT:
    SSL_free(session->ssl);
    close(session->sock);
    free(session);
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

    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    SSL_CTX *ctx = SSL_CTX_new(TLS_server_method());
    if (ctx == NULL) {
        ERR_print_errors_fp(stdout);
        return -1;
    }
    if (SSL_CTX_use_certificate_file(ctx, TLS_CERT_FILE, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stdout);
        return -1;
    }
    if (SSL_CTX_use_PrivateKey_file(ctx, TLS_KEY_FILE, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stdout);
        return -1;
    }
    if (!SSL_CTX_check_private_key(ctx)) {
        ERR_print_errors_fp(stdout);
        return -1;
    }

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
        return -1;
    }

    if (listen(server_fd, 8) != 0) {
        LOG_ERROR("server listen failed with [%d]%s", errno, strerror(errno));
        close(server_fd);
        return -1;
    }
    LOG_INFO("server listen on %s:%d ...", SERVER_HOST, SERVER_PORT);

    for (;;) {
        if (g_app_quit) {
            break;
        }

        socklen_t addr_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr *) &client_addr, &addr_len);
        if (0 < client_fd) {
            LOG_INFO("client connected from %s:%d", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
            wdm_session_t *session = (wdm_session_t *) malloc(sizeof(wdm_session_t));
            session->ctx = ctx;
            session->sock = client_fd;
            if (pthread_create(&session->tid, NULL, session_process_thread, session) != 0) {
                close(session->sock);
                free(session);
                LOG_ERROR("create session process thread failed with [%d]%s", errno, strerror(errno));
            }
        } else {
            LOG_ERROR("server accept failed with [%d]%s", errno, strerror(errno));
        }

        msleep(30);
    }

    close(server_fd);
    SSL_CTX_free(ctx);
    return 0;
}