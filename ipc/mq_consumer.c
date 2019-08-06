//
// Created by wangdm on 8/6/19.
//

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

#define MQ_NAME "/mqdemo"

mqd_t mq;
int g_quit = 0;

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


int main(int argc, char* argv[])
{
    mq = mq_open(MQ_NAME, O_RDONLY);
    if (mq < 0)
    {
        printf("open consumer mq failed with %s\n", strerror(errno));
        return 0;
    }

    pthread_t t_receive;

    pthread_create(&t_receive, NULL, receive_thread, NULL);

    sleep(20);
    g_quit = 1;

    pthread_join(t_receive, NULL);

    mq_close(mq);
    return 0;
}