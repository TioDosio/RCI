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

void reg(char *net, char *IP, char *TCP)
{
    char sendV[50];
    int F = show(1, net, IP, TCP);
    if (F >= 0 && F <= 9)
    {
        strcpy(node.id, "");
        sprintf(node.id, "0%d", F);
        printf("id: %s\n", node.id);
    }
    else if (F >= 10 && F <= 99)
    {
        strcpy(node.id, "");
        sprintf(node.id, "%d", F);
        printf("id: %s\n", node.id);
    }
    strcpy(node.vizBackup.IDv, node.id);
    strcpy(node.vizBackup.IPv, IP);
    strcpy(node.vizBackup.Portv, TCP);
    sprintf(sendV, "REG %s %s %s %s", net, node.id, IP, TCP); // REG net id IP TCP
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
    printf("%s\n\n", buffOKs);
    if (node.flagVaz > 0)
    {
        printf("entra aqui");
        client_tcp(IP, TCP);
    }
    freeaddrinfo(res);
    close(fd);
}
void unreg(char *net, char *IP, char *TCP)
{
    char sendV[50], bufOKs[10];
    sprintf(sendV, "UNREG %s %s", net, node.id); // NODES net
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
    printf("%s\n", bufOKs);
    close(fd);
    freeaddrinfo(res);
}
int show(int flagS, char *net, char *IP, char *TCP)
{
    char sendV[50];
    sprintf(sendV, "NODES %s", net); // NODES net
    struct addrinfo hints, *res;
    int fd, errcode;
    ssize_t n;
    struct sockaddr addr;
    char buffer[2500];
    strcpy(buffer, "");
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
    printf("%s\n\n", buffer);
    // meter argumentos do buffer nos arrays
    int intIDv, intID, i = 0; /////////////////////// este i pode ser descartado e substituido por node.flagVaz?
    node.flagVaz = 0;

    if (flagS == 1) // so entra se for chamada com o join //////////////////////////////////////////////////////////////////////////////////////
    {
        intID = atoi(node.id);
        char *saveptr, IDv[100][11], IPv[100][20], Portv[100][6];
        char *line = strtok_r(buffer, "\n", &saveptr);
        printf("line: %s\n", line);
        line = strtok_r(NULL, "\n", &saveptr);
        while (line != NULL)
        {
            sscanf(line, "%s %s %s", IDv[i], IPv[i], Portv[i]);
            intIDv = atoi(IDv[i]);
            while (intID == intIDv)
            {
                intID = rand() % 100;
            }
            node.flagVaz++;
            i++;
            line = strtok_r(NULL, "\n", &saveptr);
        }
        if (line == NULL && i == 0)
        {
            printf("Sou o primeiro a entrar na rede\n");
        }
        if (node.flagVaz > 0)
        {
            int r = rand() % (i);
            printf("IDv[%d]=%s\n", r, IDv[r]);
            printf("IPv[%d]=%s\n", r, IPv[r]);
            printf("Portv[%d]=%s\n", r, Portv[r]);
            strcpy(node.vizExt.IDv, IDv[r]);
            strcpy(node.vizExt.IPv, IPv[r]);
            strcpy(node.vizExt.Portv, Portv[r]);
        }
        else
        {
            intID = 111; // para nao dar erro
        }
    }
    strcpy(buffer, "");
    close(fd);
    freeaddrinfo(res);
    return intID;
}