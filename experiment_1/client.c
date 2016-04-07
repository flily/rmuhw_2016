#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define RECV_BUFFER_SIZE      1024
#define SEND_BUFFER_SIZE      1024
#define SERVER_LISTEN_PORT    5838

int main(int argc, char* argv[])
{
    struct sockaddr_in addr;
    char sbuf[SEND_BUFFER_SIZE + 4] = "";
    int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    printf("this is a client of B/S system.\n");

    if (sockfd < 0)
    {
        printf("[ERROR] Initialize socket error.\n");
        return 1;
    }

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(SERVER_LISTEN_PORT);

    if (0 > connect(sockfd, (struct sockaddr*) &addr, sizeof(addr)))
    {
        printf("Connect to server failed.\n");
        return 1;
    }

    do
    {
        char rbuf[RECV_BUFFER_SIZE + 4] = "";
        ssize_t rlen = 0, slen = 0;

        rlen = recv(sockfd, rbuf, RECV_BUFFER_SIZE, 0);
        printf("Recv %d bytes from server.\n", (int) rlen);
        if (rlen == 0)
        {
            printf("[ERROR] Recv no message.\n");
            break;
        }

        rbuf[rlen] = 0;
        printf("[RECV] %s\n", rbuf);
        printf("INPUT> ");
        scanf("%s", sbuf);

        slen = send(sockfd, sbuf, strlen(sbuf), 0);
        printf("[SEND %d] %s\n", (int) slen, sbuf);

    } while (strncmp(sbuf, "exit", 4));

    close(sockfd);

    return 0;
}

