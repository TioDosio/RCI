#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/types.h> //
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include "UDP.h"
#include "TCP.h"

void clitTCP(char *IP, char *TCP)
{
    struct addrinfo hints, *res;
    int fd, n;
    // char send[100],
    char buffer[129], *ptr; // ???tamanhos???
    // sprintf(send, "send %s %s", IP, Port);
    ssize_t nbytes, nleft, nwritten, nread;
    fd = socket(AF_INET, SOCK_STREAM, 0); // TCP socket
    if (fd == -1)
        exit(1); // error
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;                          // IPv4
    hints.ai_socktype = SOCK_STREAM;                    // TCP socket
    n = getaddrinfo("95.95.75.236", TCP, &hints, &res); // meter IP e TCP do nó
    if (n != 0)                                         /*error*/
        exit(1);
    n = connect(fd, res->ai_addr, res->ai_addrlen);
    if (n == -1) /*error*/
        exit(1);
    ptr = strcpy(buffer, "Bananinhas das boas\n");
    nbytes = strlen(ptr);
    nleft = nbytes;
    while (nleft > 0)
    {
        nwritten = write(fd, ptr, nleft);
        if (nwritten <= 0) /*error*/
            exit(1);
        nleft -= nwritten;
        ptr += nwritten;
    }
    nleft = nbytes;
    ptr = buffer;
    while (nleft > 0)
    {
        nread = read(fd, ptr, nleft);
        if (nread == -1) /*error*/
            exit(1);
        else if (nread == 0)
            break; // closed by peer
        nleft -= nread;
        ptr += nread;
    }
    nread = nbytes - nleft;
    buffer[nread] = '\0';
    printf("TCP: %s\n", buffer);
    close(fd);
}
void servTCP(char *TCP)
{
    struct addrinfo hints, *res;
    int fd, newfd, errcode;
    ssize_t n, nw;
    struct sockaddr addr;
    socklen_t addrlen;
    char *ptr, buffer[128];
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        exit(1); // error
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;       // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP socket
    hints.ai_flags = AI_PASSIVE;
    if ((errcode = getaddrinfo(NULL, TCP, &hints, &res)) != 0) /*error*/
        exit(1);
    if (bind(fd, res->ai_addr, res->ai_addrlen) == -1) /*error*/
        exit(1);
    if (listen(fd, 5) == -1) /*error*/
        exit(1);
    while (1)
    {
        addrlen = sizeof(addr);
        if ((newfd = accept(fd, &addr, &addrlen)) == -1)
            /*error*/ exit(1);
        while ((n = read(newfd, buffer, 128)) != 0)
        {
            if (n == -1) /*error*/
                exit(1);
            ptr = &buffer[0];
            while (n > 0)
            {
                if ((nw = write(newfd, ptr, n)) <= 0) /*error*/
                    exit(1);
                n -= nw;
                ptr += nw;
            }
        }
        close(newfd);
    }
    freeaddrinfo(res);
    close(fd);
    exit(0);
}
