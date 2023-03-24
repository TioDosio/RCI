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
void client_tcp(char *id, char *IP, char *TCP)
{
    char buffer[128 + 1];
    struct addrinfo hints, *res;
    int n;
    node.vizExt.fd = socket(AF_INET, SOCK_STREAM, 0); // TCP socket
    printf("socket clientTCP:%d\n", node.vizExt.fd);
    if (node.vizExt.fd == -1)
    {
        printf("erro socket tcp.c");
        exit(1); // error
    }
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM;
    // printf("IP: %s Porto: %s", node.vizExt.IPv, node.vizExt.Portv); // verificar se o IP e o porto estão bem
    n = getaddrinfo(node.vizExt.IPv, node.vizExt.Portv, &hints, &res); /// é preciso também meter o bootIP e bootPort quando for djoin
    if (n != 0)                                                        /*error*/
    {
        printf("erro getaddrinfo tcp.c");
        exit(1);
    }
    n = connect(node.vizExt.fd, res->ai_addr, res->ai_addrlen);
    if (n == -1) /*error*/
    {
        printf("error connect tcp.c\n");
        exit(1);
    }
    n = sprintf(buffer, "NEW %s %s %s\n", id, IP, TCP); // mensagem enviada ao no a que se liga com NEW ID IP PORTO
    printf("enviado por mim: %s\n", buffer);
    n = write(node.vizExt.fd, buffer, n);
    if (n == -1) /*error*/
    {
        printf("error write tcp.c\n");
        exit(1);
    }
    freeaddrinfo(res);
}

void djoin(char *net, char *id, char *IP, char *TCP, char *bootID, char *bootIP, char *bootTCP)
{
    char sendV[50], buffer[10];
    sprintf(sendV, "REG %s %s %s %s", net, bootID, bootIP, bootTCP); // REG net id IP TCP
    printf("sending: %s\n\n", sendV);
    struct addrinfo hints, *res;
    int fd, errcode;
    ssize_t n;
    struct sockaddr addr;
    socklen_t addrlen;
    fd = socket(AF_INET, SOCK_DGRAM, 0); // UDP socket
    printf("socket djoin:%d\n", fd);
    if (fd == -1) /*error*/
    {
        printf("error socket djoin");
        exit(1);
    }
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;      // IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP socket
    errcode = getaddrinfo("tejo.tecnico.ulisboa.pt", "59000", &hints, &res);
    if (errcode != 0) /*error*/
    {
        printf("error getaddrinfo djoin");
        exit(1);
    }
    n = sendto(fd, sendV, strlen(sendV), 0, res->ai_addr, res->ai_addrlen);
    if (n == -1)
    {
        printf("error send djoin"); /*error*/
        exit(1);
    }
    addrlen = sizeof(addr);

    n = recvfrom(fd, buffer, strlen(buffer), 0, &addr, &addrlen); // Recebe a resposta do servidor
    if (n == -1)
    { /*error*/
        printf("error recvfrom djoin\n");
        exit(1);
    }
    buffer[n] = '\0'; // adiciona terminador de string
    printf("received djoin: %s\n\n", buffer);
    strcpy(node.vizExt.IDv, bootID);
    strcpy(node.vizExt.IPv, bootIP);
    strcpy(node.vizExt.Portv, bootTCP);
    client_tcp(id, IP, TCP);
    close(fd);
    freeaddrinfo(res);
}
void leave(char *net, char *id, char *IP, char *TCP)
{
    unreg(net, id, IP, TCP);
    for (int i = 0; i < node.maxInter; i++) // fecha todos os sockets que se estavam a usar
    {
        if (node.vizInt[i].fd != -2)
        {
            close(node.vizInt[i].fd);
            node.vizInt[i].fd = -2;
        }
    }
    if (node.vizExt.fd != -2)
    {
        printf("EXT ID:%s IP:%s Port:%s fd:%d\n", node.vizExt.IDv, node.vizExt.IPv, node.vizExt.Portv, node.vizExt.fd);
        close(node.vizExt.fd);
        node.vizExt.fd = -2;
    }
    strcpy(node.vizExt.IDv, "");
    strcpy(node.vizExt.IPv, "");
    strcpy(node.vizExt.Portv, "");
    strcpy(node.vizBackup.IDv, "");
    strcpy(node.vizBackup.IPv, "");
    strcpy(node.vizBackup.Portv, "");
    for (int i = 0; i < node.maxInter; i++)
    {
        strcpy(node.vizInt[i].IDv, "");
        strcpy(node.vizInt[i].IPv, "");
        strcpy(node.vizInt[i].Portv, "");
    }
}