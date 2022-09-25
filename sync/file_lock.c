/**
 * Created by wangdm on 22-9-24.
**/

#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>

#include "log.h"

#define msleep(ms) usleep((ms)*1000)

#define USE_SHM 1
#if USE_SHM
#define LOCK_SHM "shm_lock"
#else
#define LOCK_FILE_PATH PROJECT_SOURCE_ROOT"/data/"
#define LOCK_FILE LOCK_FILE_PATH"file.lck"
#endif

static int g_app_quit = 0;

static int file_wlock(int fd) {
    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    lock.l_pid = getpid();
    int ret = fcntl(fd, F_SETLKW, &lock);
    if (ret == -1) {
        LOG_ERROR("fcntl failed with [%d]%s", errno, strerror(errno));
        return -1;
    }
    return 0;
}

static int file_rlock(int fd) {
    struct flock lock;
    lock.l_type = F_RDLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    lock.l_pid = getpid();
    int ret = fcntl(fd, F_SETLKW, &lock);
    if (ret == -1) {
        LOG_ERROR("fcntl failed with [%d]%s", errno, strerror(errno));
        return -1;
    }
    return 0;
}

static int file_unlock(int fd) {
    struct flock lock;
    lock.l_type = F_UNLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    lock.l_pid = getpid();
    int ret = fcntl(fd, F_SETLKW, &lock);
    if (ret == -1) {
        LOG_ERROR("fcntl failed with [%d]%s", errno, strerror(errno));
        return -1;
    }
    return 0;
}

static void process_write(int fd) {
    for (;;) {
        if (g_app_quit) {
            break;
        }
        if (file_wlock(fd) == 0) {
            LOG_INFO("get write lock");
            sleep(2);
            file_unlock(fd);
        }
        msleep(30);
    }
}

static void process_read(int fd) {

    for (;;) {
        if (g_app_quit) {
            break;
        }
        if (file_rlock(fd) == 0) {
            LOG_INFO("get read lock");
            sleep(2);
            file_unlock(fd);
        }
        msleep(30);
    }
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

#if USE_SHM
    int fd = shm_open(LOCK_SHM, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
#else
    int fd = open(LOCK_FILE, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
#endif
    if (fd < 0) {
        LOG_FATAL("open lock file failed with [%d]%s", errno, strerror(errno));
        return -1;
    }

    pid_t pid = fork();
    if (pid == -1) {
        LOG_FATAL("linux fork process failed with [%d]%s", errno, strerror(errno));
        return -1;
    } else if (pid == 0) {
        /// father process
        process_write(fd);
    } else {
        /// child process
        process_read(fd);
    }

    close(fd);
#if USE_SHM
    shm_unlink(LOCK_SHM);
#else
    unlink(LOCK_FILE);
#endif
    return 0;
}