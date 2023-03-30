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

void client_tcp(char *IP, char *TCP)
{
    char buffer[128] = "";
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
    n = getaddrinfo(node.vizExt.IPv, node.vizExt.Portv, &hints, &res);
    if (n != 0) /*error*/
    {
        printf("erro getaddrinfo tcp.c");
        exit(1);
    }
    n = connect(node.vizExt.fd, res->ai_addr, res->ai_addrlen);
    printf("connect clientTCP:%d\n", node.vizExt.fd); ///////////////////////////
    if (n == -1)                                      /*error*/
    {
        printf("error connect tcp.c\n");
        exit(1);
    }
    sprintf(buffer, "NEW %s %s %s\n", node.id, IP, TCP); // mensagem enviada ao no a que se liga com NEW ID IP PORTO
    printf("enviado por mim: %s\n", buffer);
    write(node.vizExt.fd, buffer, strlen(buffer));
    freeaddrinfo(res);
}

void djoin(char *net, char *IP, char *TCP, char *bootID, char *bootIP, char *bootTCP)
{
    if (strcmp(node.id, bootID) == 0)
    {
        printf("ID igual ao do boot\n");
    }
    else
    {
        strcpy(node.vizExt.IDv, bootID);
        strcpy(node.vizExt.IPv, bootIP);
        strcpy(node.vizExt.Portv, bootTCP);
        client_tcp(IP, TCP);
    }
}
void leave(char *net, char *IP, char *TCP)
{
    unreg(net, IP, TCP);
    for (int i = 0; i < node.maxInter; i++)
    {
        if (node.vizInt[i].fd != -2)
        {
            printf("closing vizInt[%d] %s\n", node.vizInt[i].fd, node.vizInt[i].IDv);
            close(node.vizInt[i].fd);
            node.vizInt[i].fd = -2;
        }
    }
    if (node.vizExt.fd != -2)
    {
        printf("closing vizExt\n");
        close(node.vizExt.fd);
        node.vizExt.fd = -2;
    }
    for (int i = 0; i < 100; i++)
    {
        node.tabExp[i] = -2;
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