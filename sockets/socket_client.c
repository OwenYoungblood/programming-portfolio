#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netdb.h>

int main(int argc, char *argv[])
{
    if(argc < 4)
    {
        return 1;
    }
    struct addrinfo hints = {0};
    struct addrinfo *res;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    int address = getaddrinfo(argv[1], argv[2], &hints, &res);
    if(address != 0)
    {
        printf("fail\n");
        return 1;
    }
    int sfd = socket(res->ai_family, res->ai_socktype, 0);
    if(sfd == -1)
    {
        perror("socket");
        return 1;
    }
    if(connect(sfd, res->ai_addr, res->ai_addrlen) == -1)
    {
        perror("connect");
        return 1;
    }
    freeaddrinfo(res);
    ssize_t bytes;
    char data[256];
    sprintf(data, "GET %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", argv[3], argv[1]);
    char buffer[4096];
    bytes = send(sfd, data, strlen(data), MSG_NOSIGNAL);
    if(bytes == -1)
    {
        perror("send");
        return 1;
    }
    int done = 0;
    while((bytes = recv(sfd, buffer, sizeof(buffer) - 1, 0)) > 0)
    {
        buffer[bytes] = '\0';
        if(!done)
        {
            char *body = strstr(buffer, "\r\n\r\n");
            if(body)
            {
                body += 4;
                fwrite(body, 1, bytes - (body - buffer), stdout);
                done = 1;
            }
        }
        else
        {
            fwrite(buffer, 1, bytes, stdout);
        }
    }
    if(bytes == -1)
    {
        perror("recv");
    }
    return 0;
}
