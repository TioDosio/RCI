#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include "UDP.h"
#include "TCP.h"
#include "fs.h"

extern struct NO node; // ter variavel global em varios ficheiros
// extern char TCP[50];   // ter variavel global em varios ficheiros

void client_tcp()
{
    ssize_t nbytes, nleft, nwritten, nread;
    char *ptr, buffer[128 + 1];
    struct addrinfo hints, *res;
    int fd, n;
    fd = socket(AF_INET, SOCK_STREAM, 0); // TCP socket
    if (fd == -1)
    {
        printf("erro socket tcp.c");
        exit(1); // error
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM;
    printf("IP: %s Porto: %s", node.vizExt.IPv, node.vizExt.Portv);
    n = getaddrinfo(node.vizExt.IPv, node.vizExt.Portv, &hints, &res); /// erro aqui
    if (n != 0)                                                        /*error*/
    {
        printf("erro getaddrinfo tcp.c");
        exit(1);
    }
    n = connect(fd, res->ai_addr, res->ai_addrlen);
    if (n == -1) /*error*/
    {
        printf("error connect tcp.c\n");
        exit(1);
    }
    else
    {
        printf("client not error\n");
    }
    ptr = strcpy(buffer, "Hello manos bros!\n");
    nbytes = 7;
    nleft = nbytes;
    while (nleft > 0)
    {
        nwritten = write(fd, ptr, nleft);
        if (nwritten <= 0) /*error*/
        {
            printf("erro write tcp.c");
            exit(1);
        }
        nleft -= nwritten;
        ptr += nwritten;
    }
    nleft = nbytes;
    ptr = buffer;
    while (nleft > 0)
    {
        nread = read(fd, ptr, nleft);
        if (nread == -1) /*error*/
        {
            printf("erro read tcp.c");
            exit(1);
        }
        else if (nread == 0)
            break; // closed by peer
        nleft -= nread;
        ptr += nread;
    }
    nread = nbytes - nleft;
    buffer[nread] = '\0';
    printf("echo: %s\n", buffer);
}