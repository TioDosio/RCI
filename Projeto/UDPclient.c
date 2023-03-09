// test.c
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
int main(void)
{
    struct addrinfo
    {                             // (item in a linked list)
        int ai_flags;             // additional options
        int ai_family;            // address family
        int ai_socktype;          // socket type
        int ai_protocol;          // protocol
        socklen_t ai_addrlen;     // address length (bytes)
        struct sockaddr *ai_addr; // socket address
        char *ai_canonname;       // canonical hostname
        struct addrinfo *ai_next; // next item
    };
    struct addrinfo hints, *res;
    int fd, errcode;
    ssize_t n;
    char SendV[50], net[] = "666"; // O que é enviado ao servidor UDP
    sprintf(SendV, "NODES %s", net);
    struct sockaddr addr;
    ssize_t nbytes, nleft, nwritten, nread;
    char *ptr, buffer[128 + 1];
    int socket(int domain, int type, int protocol);
    ssize_t sendto(int s, const void *buf, size_t len, int flags,
                   const struct sockaddr *dest_addr, socklen_t addrlen);
    socklen_t addrlen;
    fd = socket(AF_INET, SOCK_DGRAM, 0); // UDP socket
    if (fd == -1)                        /*error*/
        exit(1);
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;      // IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP socket
    // errcode = getaddrinfo("tejo.tecnico.ulisboa.pt", "59000", &hints, &res);
    if (errcode != 0) /*error*/
        exit(1);
    n = sendto(fd, SendV, strlen(SendV), 0, res->ai_addr, res->ai_addrlen);
    printf("%s", struct sockaddr * res->ai_addr);
    if (n == -1) /*error*/
        exit(1);
    addrlen = sizeof(addr);
    n = recvfrom(fd, buffer, 128, 0, &addr, &addrlen);
    if (n == -1) /*error*/
        exit(1);
    buffer[n] = '\0';
    printf("echo: %s\n", buffer);

    ptr = strcpy(buffer, "Bananinhas das boas\n");
    nbytes = 7;
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
    printf("echo: %s\n", buffer);
    close(fd);
    exit(0);
}