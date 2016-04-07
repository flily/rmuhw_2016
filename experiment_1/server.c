#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define RECV_BUFFER_SIZE      1024
#define SEND_BUFFER_SIZE      1024
#define SERVER_LISTEN_PORT    5838

int serve_client(int serve_id, int sock)
{
    char rbuf[RECV_BUFFER_SIZE + 4] = "";
    char sbuf[SEND_BUFFER_SIZE + 4] = "";

    sprintf(sbuf, "hello world at %d", serve_id);
    send(sock, sbuf, strlen(sbuf), 0);

    while (0 != strncmp(rbuf, "exit", 4))
    {
        ssize_t slen = 0;
        ssize_t rlen = recv(sock, rbuf, RECV_BUFFER_SIZE, 0);
        if (rlen == 0)
        {
            printf("[%d] recv no message and exit.\n", serve_id);
            return 1;
        }

        rbuf[rlen] = 0;
        printf("[%d] RECV: %s\n", serve_id, rbuf);
        sprintf(sbuf, "total recv %d bytes.", (int) rlen);
        slen = send(sock, sbuf, strlen(sbuf), 0);
        printf("[%d] SEND: %s\n", serve_id, sbuf);
    }

    return 0;
}


int main(int argc, char* argv[])
{
    struct sockaddr_in server_addr;
    int sockfd= socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    int serve_id = 0;

    if (sockfd < 0)
    {
        printf("[ERROR] Initialize socket error.\n");
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_LISTEN_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (0 > bind(sockfd, (struct sockaddr*) &server_addr, sizeof(server_addr)))
    {
        printf("[ERROR] bind port %d failed.\n", SERVER_LISTEN_PORT);
        return 2;
    }

    if (0 > listen(sockfd, 10))
    {
        printf("[ERROR] listen failed.\n");
        return 3;
    }

    printf("start server at localhost:%d\n", SERVER_LISTEN_PORT);

    while (1)
    {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        int sock = accept(sockfd, (struct sockaddr*) &client_addr, &addr_len);
        int pid = 0;

        if (sock < 0)
        {
            printf("[ERROR] accept from client failed.\n");
            break;
        }

        serve_id++;

        if ((pid = fork()) > 0)   /* parent process */
        {
            printf("Accept new client %d in process %d\n", serve_id, pid);
            close(sock);
            continue;
        }

        /* child process */
        close(sockfd);
        serve_client(serve_id, sock);
        printf("[%d] client exit..\n", serve_id);
        return 0;
    }

    close(sockfd);
    printf("exit...\n");

    return 0;
}

