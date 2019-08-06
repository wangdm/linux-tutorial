//
// Created by wangdm on 8/6/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <mqueue.h>
#include <errno.h>

#include <pthread.h>
#include <signal.h>

#include <sys/syscall.h>

#define MQ_NAME "/mqdemo"
#define MQ_MESSAGE_FMT "Hello, Receiver; count[%d]."

#define gettid() syscall(SYS_gettid)

mqd_t mq;
int g_quit = 0;

void* send_thread(void* arg)
{
    int count = 0;
    while (g_quit == 0)
    {
        char buf[128];
        sprintf(buf, MQ_MESSAGE_FMT, ++count);
        int wsize = mq_send(mq, buf, strlen(buf), 0);
        if (wsize < 0)
        {
            printf("mq_send error with %s\n", strerror(errno));
        }
        else
        {
            printf("send message ...\n");
        }

        sleep(1);
    }
    return NULL;
}

void* receive_thread(void* arg)
{
    struct mq_attr attr;
    mq_getattr(mq, &attr);
    printf("####################\n");
    printf("# mq_msgsize: %ld\n", attr.mq_msgsize);
    printf("# mq_maxmsg: %ld\n", attr.mq_maxmsg);
    printf("# mq_curmsgs: %ld\n", attr.mq_curmsgs);
    printf("# mq_flags: %ld\n", attr.mq_flags);
    printf("####################\n");

    while (g_quit == 0)
    {
        char buf[128];
        int rsize = mq_receive(mq, buf, attr.mq_msgsize, NULL);
        if (rsize < 0)
        {
            printf("mq_receive error with [%d]%s\n", errno, strerror(errno));
        }
        if (rsize > 0){
            printf("%s", buf);
            printf("\n");
        }
    }
    return NULL;
}

void signal_handler(int signum)
{
    //pid_t pid =  getpid();
    pid_t tid = gettid();
    //pthread_t tid = pthread_self();
    printf("Thread[%d] receive signal %d\n", tid, signum);
}

int main(int argc, char* argv[])
{
    struct sigaction sig;
    sig.sa_handler = signal_handler;
    sig.sa_flags = 0;
    sigaction(SIGUSR1, &sig, NULL);

    mq = mq_open(MQ_NAME, O_RDWR|O_CREAT, 0666, NULL);
    if (mq < 0)
    {
        printf("open producer mq failed with %s\n", strerror(errno));
        return 0;
    }
    else
    {
        printf("open mq success\n");
    }

    pthread_t t_send;
    pthread_t t_receive;

    pthread_create(&t_send, NULL, send_thread, NULL);
    pthread_create(&t_receive, NULL, receive_thread, NULL);

    sleep(20);
    g_quit = 1;

    pthread_kill(t_send, SIGUSR1);
    pthread_kill(t_receive, SIGUSR1);

    //kill(0, SIGUSR1);

    pthread_join(t_send, NULL);
    pthread_join(t_receive, NULL);

    mq_close(mq);
    return 0;
}