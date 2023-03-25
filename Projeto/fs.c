#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include "UDP.h"
#include "TCP.h"
#include "fs.h"
extern struct NO node;

void create(char *name)
{
    strcpy(node.names[node.flagName], name);
    printf("Posição:%d, name:%s\n", node.flagName, node.names[node.flagName]);
}
void delete(char *name)
{
    for (int i = 0; i < node.flagName; i++) // passa por todos os nomes já criados
    {
        if (strcmp(node.names[i], name) == 0) // procura o nome
        {
            strcpy(node.names[i], "\0");            // apaga o nome
            for (int j = i; j < node.flagName; j++) // shift left
            {
                strcpy(node.names[j], node.names[j + 1]);
            }
        }
    }
}
void showNames()
{
    for (int i = 0; i < node.flagName; i++)
    {
        printf("Posição:%d, name:%s\n", i, node.names[i]);
    }
}
void get(char *dest, char *id, char *name)
{
    // QUERY dest orig name
    char bufsend[100];
    int n, l;
    l = sprintf(bufsend, "QUERY %s %s %s\n", dest, id, name);
    printf("bufsend:%s\n", bufsend);
    printf("AAAAAAFD:%d\n", node.vizExt.fd);
    n = write(node.vizExt.fd, bufsend, l);
    if (n == -1) /*error*/
    {
        printf("error write ext tcp.c\n");
        exit(1);
    }
    for (int i = 0; i < node.maxInter; i++) // node.maxInter só é incrementado depois de um inverno ser
    {
        printf("4BAN FD:%d\n", node.vizInt[i].fd);
        n = write(node.vizInt[i].fd, bufsend, l);
        if (n == -1) /*error*/
        {
            printf("error write int tcp.c\n");
            exit(1);
        }
    }
}
void showTopo() // node.maxInter para o for dos viz internos
{
    printf("Vizinho Externo:   id:%s ip:%s porto:%s fd:%d\n", node.vizExt.IDv, node.vizExt.IPv, node.vizExt.Portv, node.vizExt.fd);
    for (int i = 0; i < node.maxInter; i++)
    {
        printf("Vizinho Interno %d:   id:%s ip:%s porto:%s fd:%d\n", i, node.vizInt[i].IDv, node.vizInt[i].IPv, node.vizInt[i].Portv, node.vizInt[i].fd);
    }
    printf("Vizinho Backup:   id:%s ip:%s porto:%s\n", node.vizBackup.IDv, node.vizBackup.IPv, node.vizBackup.Portv);
}
void showRouting()
{
    printf("Routing table:\n");
    for (int i = 0; i < 100; i++)
    {
        if (node.tabExp[i] != -1)
        {
            printf("TabelaExp[%d]:%d\n", i, node.tabExp[i]);
        }
    }
}