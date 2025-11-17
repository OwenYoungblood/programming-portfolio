#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/un.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>

void handler(int signum)
{
    (void)signum;
}
int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage: %s <socket file>\n", argv[0]);
        return 1;
    }
    signal(SIGINT, handler);
    unlink(argv[1]);
    struct sockaddr_un sun = {0};
    struct sockaddr_un client_addr;
    struct addrinfo hints = {0};
    struct addrinfo *res;
    hints.ai_socktype = SOCK_STREAM;
    socklen_t client_addr_len;
    int srvsock;
    int clnsock;
    char data[128];
    ssize_t bytes;

    srvsock = socket(AF_UNIX, SOCK_STREAM, 0);
    if(srvsock == -1)
    {
        perror("socket");
        return 1;
    }
    sun.sun_family = AF_UNIX;
    strncpy(sun.sun_path, argv[1], sizeof(sun.sun_path) - 1);
    if(bind(srvsock, (struct sockaddr *)&sun, sizeof(sun)) == -1)
    {
        perror("bind");
        return 1;
    }
    if(listen(srvsock, 5) == -1)
    {
        perror("listen");
        close(srvsock);
        unlink(argv[1]);
        return 1;
    }
    fcntl(srvsock, F_SETFL, O_NONBLOCK);
    struct timespec wait;
    wait.tv_sec = 0;
    wait.tv_nsec = 100000;
    for(;;)
    {
        client_addr_len = sizeof(client_addr);
        clnsock = accept(srvsock, (struct sockaddr *)&client_addr, &client_addr_len);
        if(clnsock != -1)
        {
            bytes = send(clnsock, "HELLO", 5, MSG_NOSIGNAL);
            bytes = recv(clnsock, data, sizeof(data) - 1, MSG_NOSIGNAL);
            if(bytes > 0 && strncmp(data, "HELLO", 5) == 0)
            {
                for(;;)
                {
                    bytes = recv(clnsock, data, sizeof(data) - 1, MSG_NOSIGNAL);
                    if(bytes <= 0)
                    {
                        break;
                    }
                    
                    data[bytes] = '\0';
                    if(strncmp(data, "BYE", 3) == 0)
                    {
                        break;
                    }
                    else if(strncmp(data, "A\t", 2) == 0)
                    {
                        hints.ai_family = AF_INET;
                        char hostname[128];
                        strncpy(hostname, data + 2, sizeof(hostname) - 1);
                        hostname[sizeof(hostname) - 1] = '\0';
                        int address = getaddrinfo(hostname, NULL, &hints, &res);
                        if(address != 0)
                        {
                            send(clnsock, "NOTFOUND", 8, MSG_NOSIGNAL);
                        }
                        else
                        {
                            struct sockaddr_in *sin = (struct sockaddr_in *)res->ai_addr;
                            unsigned char *broken_up = (unsigned char *)&sin->sin_addr.s_addr;
                            char address_str[64];
                            snprintf(address_str, sizeof(address_str), "OK\t%03d.%03d.%03d.%03d", broken_up[0], broken_up[1], broken_up[2], broken_up[3]);
                            send(clnsock, address_str, strlen(address_str), MSG_NOSIGNAL);
                            freeaddrinfo(res);
                        }

                    }
                    else if(strncmp(data, "AAAA\t", 5) == 0)
                    {
                        hints.ai_family = AF_INET6;
                        char hostname[128];
                        strncpy(hostname, data + 5, sizeof(hostname) - 1);
                        int address = getaddrinfo(hostname, NULL, &hints, &res);
                        if(address != 0)
                        {
                            send(clnsock, "NOTFOUND", 8, MSG_NOSIGNAL);
                        }
                        else
                        {
                            struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)res->ai_addr;
                            unsigned char *b = (unsigned char *)&sin6->sin6_addr;
                            char address_str[64];
                            snprintf(address_str, sizeof(address_str), "OK\t%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x",
                                                                    (b[0] << 8) | b[1], (b[2] << 8) | b[3], (b[4] << 8) | b[5], (b[6] << 8) | b[7],
                                                                    (b[8] << 8) | b[9], (b[10] << 8) | b[11], (b[12] << 8) | b[13], (b[14] << 8) | b[15]);   
                            send(clnsock, address_str, strlen(address_str), MSG_NOSIGNAL);
                            freeaddrinfo(res);
                        }
                    }
                }
            }
            close(clnsock);
        }  
        if(nanosleep(&wait, NULL) == -1)
        {
            break;
        }  
    }
    close(srvsock);
    unlink(argv[1]);
    return 0;
}
