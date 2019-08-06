//
// Created by wangdm on 8/6/19.
//

#include <stdio.h>
#include <stdlib.h>

#include <uv.h>

#define DEFAULT_IP "0.0.0.0"
#define DEFAULT_PORT 3000

void on_allocation(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf)
{
    buf->base = malloc(suggested_size);
    buf->len = suggested_size;
}

void on_read(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf)
{
    if(0 > nread)
    {
        if(UV_EOF == nread)
        {
            printf("client disconnect.\n");
        }
        else
        {
            fprintf(stderr, "on_read failed with [%s]%s\n", uv_err_name(nread), uv_strerror(nread));
        }
    }

    printf("on_read, nread:%ld, buf_base:0x%08X, buf_len:%ld\n", nread, buf->base, buf->len);
    if (0 < nread)
    {
    }
}

void on_connection(uv_stream_t* server, int status)
{
    if (0 != status)
    {
        fprintf(stderr, "client connect failed with [%s]%s\n", uv_err_name(status), uv_strerror(status));
        return;
    }

    uv_tcp_t client;
    uv_tcp_init(uv_default_loop(), &client);

    if(uv_accept(server, (uv_stream_t*)&client) < 0)
    {

    }

    printf("client connected\n");
    uv_read_start((uv_stream_t*)&client, on_allocation,on_read);
}

int main(int argc, char* argv[])
{
    uv_loop_t* loop = uv_default_loop();

    uv_tcp_t sock;
    uv_tcp_init(loop, &sock);

    struct sockaddr_in addr;
    uv_ip4_addr(DEFAULT_IP, DEFAULT_PORT, &addr);

    uv_tcp_bind(&sock, (struct sockaddr*)&addr, 0);
    uv_listen((uv_stream_t*)&sock, 8, on_connection);

    uv_run(loop, UV_RUN_DEFAULT);
}