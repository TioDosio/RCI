#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <time.h>
#include "UDP.h"
#include "TCP.h"
#include "fs.h"

extern struct NO node; // ter variavel global em varios ficheiros

int reg(char *net, char *id, char *IP, char *TCP)
{
    char sendV[50];
    int F = show(1, net, id, IP, TCP), fd_ext = -1;
    if (F >= 0 && F <= 9)
    {
        strcpy(id, "");
        sprintf(id, "0%d", F);
        printf("id: %s\n", id);
    }
    else if (F >= 10 && F <= 99)
    {
        strcpy(id, "");
        sprintf(id, "%d", F);
        printf("id: %s\n", id);
    }
    sprintf(sendV, "REG %s %s %s %s", net, id, IP, TCP); // REG net id IP TCP
    printf("sending: %s\n\n", sendV);
    struct addrinfo hints, *res;
    int fd, errcode;
    ssize_t n;
    struct sockaddr addr;
    char buffOKs[8]; // max = OKUNREG
    socklen_t addrlen;
    fd = socket(AF_INET, SOCK_DGRAM, 0); // UDP socket
    if (fd == -1)                        /*error*/
        exit(1);
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;      // IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP socket
    errcode = getaddrinfo("tejo.tecnico.ulisboa.pt", "59000", &hints, &res);
    if (errcode != 0) /*error*/
    {
        printf("erro get addrinfo reg.c");
        exit(1);
    }
    n = sendto(fd, sendV, strlen(sendV), 0, res->ai_addr, res->ai_addrlen);
    if (n == -1) /*error*/
    {
        printf("erro send reg.c");
        exit(1);
    }
    addrlen = sizeof(addr);

    n = recvfrom(fd, buffOKs, sizeof(buffOKs), 0, &addr, &addrlen); // Recebe a resposta do servidor
    if (n == -1)
    {
        printf("erro recv reg.c");
        exit(1);
    }
    buffOKs[n] = '\0'; // adiciona terminador de string
    printf("received: %s\n\n", buffOKs);
    if (node.flagVaz > 1)
    {
        fd_ext = client_tcp(id, IP, TCP);
    }
    return fd_ext;
    freeaddrinfo(res);
}
void unreg(char *net, char *id, char *IP, char *TCP)
{
    char sendV[50], bufOKs[10];
    sprintf(sendV, "UNREG %s %s", net, id); // NODES net
    printf("sending: %s\n", sendV);
    struct addrinfo hints, *res;
    int fd, errcode;
    ssize_t n;
    struct sockaddr addr;
    socklen_t addrlen;
    fd = socket(AF_INET, SOCK_DGRAM, 0); // UDP socket
    if (fd == -1)                        /*error*/
    {
        printf("erro socket leave.c\n");
        exit(1);
    }
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;      // IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP socket
    errcode = getaddrinfo("tejo.tecnico.ulisboa.pt", "59000", &hints, &res);
    if (errcode != 0) /*error*/
    {
        printf("erro get addrinfo leave.c\n");
        exit(1);
    }
    n = sendto(fd, sendV, strlen(sendV), 0, res->ai_addr, res->ai_addrlen);
    if (n == -1) /*error*/
    {
        printf("erro send leave.c\n");
        exit(1);
    }
    addrlen = sizeof(addr);

    n = recvfrom(fd, bufOKs, 10, 0, &addr, &addrlen); // Recebe a resposta do servidor
    if (n == -1)                                      /*error*/
    {
        printf("erro recv leave.c\n");
        exit(1);
    }
    bufOKs[n] = '\0'; // adiciona terminador de string
    printf("received: %s\n\n", bufOKs);
    close(fd);
    freeaddrinfo(res);
}
int show(int flagS, char *net, char *id, char *IP, char *TCP)
{
    char sendV[50];
    sprintf(sendV, "NODES %s", net); // NODES net
    printf("sending: %s\n\n", sendV);
    struct addrinfo hints, *res;
    int fd, errcode;
    ssize_t n;
    struct sockaddr addr;
    char buffer[2500];
    socklen_t addrlen;
    fd = socket(AF_INET, SOCK_DGRAM, 0); // UDP socket
    if (fd == -1)                        /*error*/
    {
        printf("erro socket show\n");
        exit(1);
    }
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;      // IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP socket
    errcode = getaddrinfo("tejo.tecnico.ulisboa.pt", "59000", &hints, &res);
    if (errcode != 0) /*error*/
    {
        printf("erro get addrinfo show\n");
        exit(1);
    }
    n = sendto(fd, sendV, strlen(sendV), 0, res->ai_addr, res->ai_addrlen);
    if (n == -1) /*error*/
    {
        printf("erro send show\n");
        exit(1);
    }
    addrlen = sizeof(addr);

    n = recvfrom(fd, buffer, 2500, 0, &addr, &addrlen); // Recebe a resposta do servidor
    if (n == -1)
    { /*error*/
        printf("erro read show\n");
        exit(1);
    }
    buffer[n] = '\0'; // adiciona terminador de string
    printf("received: %s\n\n", buffer);
    // meter argumentos do buffer nos arrays
    int intIDv, intID;
    node.flagVaz = 0;

    if (flagS == 1) // so entra se for o join
    {               /*Dá para fazer isto com strings mudar*/
        intID = atoi(id);
        char *saveptr, IDv[11], IPv[20], Portv[6];
        char *line = strtok_r(buffer, "\n", &saveptr);
        printf("line: %s\n", line);
        while (line != NULL)
        {
            sscanf(line, "%s %s %s", IDv, IPv, Portv);
            intIDv = atoi(IDv);
            line = strtok_r(NULL, "\n", &saveptr);
            while (intID == intIDv)
            {
                intID = rand() % 100;
                // printf("ID already exists, new ID: %d", intID);
            }
            node.flagVaz++;
            printf("flagVaz: %d", node.flagVaz);
        }
        if (line == NULL)
        {
            printf("\n%s %s %s\n", IDv, IPv, Portv);
        }
        if (node.flagVaz > 1)
        {
            strcpy(node.vizExt.IDv, IDv);
            strcpy(node.vizExt.IPv, IPv);
            strcpy(node.vizExt.Portv, Portv);
        }
        else
        {
            intID = 111; // para nao dar erro
        }
    }

    close(fd);
    freeaddrinfo(res);
    return intID;
}