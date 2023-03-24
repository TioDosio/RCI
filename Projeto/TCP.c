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
extern int maxInter;   // Mudar

void client_tcp(char *id, char *IP, char *TCP)
{
    char buffer[128 + 1];
    struct addrinfo hints, *res;
    int n;
    node.vizExt.fd = socket(AF_INET, SOCK_STREAM, 0); // TCP socket
    if (node.vizExt.fd == -1)
    {
        printf("erro socket tcp.c");
        exit(1); // error
    }
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM;
    n = getaddrinfo(node.vizExt.IPv, node.vizExt.Portv, &hints, &res);
    if (n != 0) /*error*/
    {
        printf("erro getaddrinfo tcp.c");
        exit(1);
    }
    n = connect(node.vizExt.fd, res->ai_addr, res->ai_addrlen); // connect to server
    if (n == -1)                                                /*error*/
    {
        printf("error connect tcp.c\n");
        exit(1);
    }
    char buf[100] = "";
    n = sprintf(buffer, "NEW %s %s %s\n", id, IP, TCP); // mensagem enviada ao no a que se liga com NEW ID IP PORTO
    n = write(node.vizExt.fd, buffer, n);               // escreve no socket
    if (n == -1)                                        /*error*/
    {
        printf("error write tcp.c\n");
        exit(1);
    }
    n = read(node.vizExt.fd, buf, 100); // le do socket EXTERN ID IP PORTO
    if (n == -1)                        /*error*/
    {
        printf("error read tcp.c\n");
        exit(1);
    }
    if (n == -1)
    {
        printf("erro read client_tcp\n");
    }
    sscanf(buf, "EXTERN %s %s %s", node.vizBackup.IDv, node.vizBackup.IPv, node.vizBackup.Portv); // guarda o vizinho externo do externo
    freeaddrinfo(res);
}

void djoin(char *net, char *id, char *IP, char *TCP, char *bootID, char *bootIP, char *bootTCP)
{
    char sendV[50], buffer[10];
    sprintf(sendV, "REG %s %s %s %s", net, bootID, bootIP, bootTCP); // REG net id IP TCP
    struct addrinfo hints, *res;
    int fd, errcode;
    ssize_t n;
    struct sockaddr addr;
    socklen_t addrlen;
    fd = socket(AF_INET, SOCK_DGRAM, 0); // UDP socket
    if (fd == -1)                        /*error*/
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
    n = sendto(fd, sendV, strlen(sendV), 0, res->ai_addr, res->ai_addrlen); // Envia a mensagem ao servidor
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
    buffer[n] = '\0';                // adiciona terminador de string
    strcpy(node.vizExt.IDv, bootID); // assume que é o primmeiro/segundo da rede
    strcpy(node.vizExt.IPv, bootIP);
    strcpy(node.vizExt.Portv, bootTCP);
    client_tcp(id, IP, TCP);
    close(fd);
    freeaddrinfo(res);
}
void leave(char *net, char *id, char *IP, char *TCP, int maxInter)
{
    unreg(net, id, IP, TCP);
    for (int i = 0; i < maxInter; i++) // fecha todos os sockets que se estavam a usar
    {
        if (node.vizInt[i].fd != -2)
        {
            close(node.vizInt[i].fd);
            node.vizInt[i].fd = -2;
        }
    }
    if (node.vizExt.fd != -2)
    {
        close(node.vizExt.fd);
        node.vizExt.fd = -2;
    }
    strcpy(node.vizExt.IDv, "");
    strcpy(node.vizExt.IPv, "");
    strcpy(node.vizExt.Portv, "");
    strcpy(node.vizBackup.IDv, "");
    strcpy(node.vizBackup.IPv, "");
    strcpy(node.vizBackup.Portv, "");
    for (int i = 0; i < maxInter; i++)
    {
        strcpy(node.vizInt[i].IDv, "");
        strcpy(node.vizInt[i].IPv, "");
        strcpy(node.vizInt[i].Portv, "");
    }
}