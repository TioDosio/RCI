#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include "UDP.h"

void Reg(int PauloBranco, char *net, char *id, char *IP, char *TCP)
{
    char sendV[50];
    char sendS[10];
    char bufReg[2500], bufNodes[2500]; // bufReg[]= "OKREG" or "OKUNREG" bufNodes[]= todos os nós e info
    struct addrinfo hints, *res;
    int fd, errcode;
    ssize_t n;
    struct sockaddr addr;
    int socket(int domain, int type, int protocol);
    ssize_t sendto(int s, const void *buf, size_t len, int flags,
                   const struct sockaddr *dest_addr, socklen_t addrlen);
    socklen_t addrlen;
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
    fd = socket(AF_INET, SOCK_DGRAM, 0); // UDP socket
    if (fd == -1)                        /*error*/
        exit(1);
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;      // IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP socket
    errcode = getaddrinfo("tejo.tecnico.ulisboa.pt", "59000", &hints, &res);
    if (errcode != 0) /*error*/
        exit(1);

    if (PauloBranco == 1) // UnResgistar
    {
        // sscanf(sendV, "%s %s %s", PauloBranco, net, id);  // UNREG net id
        sprintf(sendV, "UNREG %s %s", net, id); // NODES net
    }
    else if (PauloBranco == 0) // Registar mas pergunta pelos nós antes
    {
        // NODES net
        n = sendto(fd, sendS, strlen(sendS), 0, res->ai_addr, res->ai_addrlen);
        if (n == -1) /*error*/
            exit(1);
        addrlen = sizeof(addr);
        n = recvfrom(fd, bufNodes, 2500, 0, &addr, &addrlen); // Recebe a resposta do servidor
        if (n == -1)
        { /*error*/
            exit(1);
        }
        bufNodes[n] = '\0'; // adiciona terminador de string
        printf("received: %s\n", bufNodes);
        sprintf(sendV, "REG %s %s %s %s", net, id, IP, TCP); // REG net id IP TCP
    }
    else if (PauloBranco == 2)
    {
        sprintf(sendS, "NODES %s", net); // NODES net
    }
    char *saveptr, IDv[3], IPv[20], Portv[6]; // not array
    char *line = strtok_r(buffer, "\n", &saveptr);
    while (line != NULL)
    {
        sscanf(line, "%s %s %s", IDv, IPv, Portv);
        srand(time(NULL));
        int auxIDv = atoi(IDv);
        int auxid = atoi(id);
        while (auxIDv == auxid)
        {
            id = rand() % 100;
            printf("ID repetido, novo ID: %d\n", auxid);
        }
        printf("NOVO: %d\n", auxid);

        line = strtok_r(NULL, "\n", &saveptr);
    }
    if (line == NULL)
    {
        printf("\n%s %s %s\n", IDv, IPv, Portv);
    }

    printf("sending: %s\n\n", sendV);
    n = sendto(fd, sendV, strlen(sendV), 0, res->ai_addr, res->ai_addrlen);
    if (n == -1) /*error*/
        exit(1);
    addrlen = sizeof(addr);
    n = recvfrom(fd, bufReg, 2500, 0, &addr, &addrlen); // Recebe a resposta do servidor
    if (n == -1)
    { /*error*/
        exit(1);
    }
    bufReg[n] = '\0'; // adiciona terminador de string
    printf("%s\n", bufReg);

    close(fd);
    freeaddrinfo(res);
}