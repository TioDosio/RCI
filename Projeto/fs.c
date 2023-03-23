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
extern int maxclits; // Mudar

void create(char *name, int flagName)
{
    strcpy(node.names[flagName], name);
    printf("Posição:%d, name:%s\n", flagName, node.names[flagName]);
}
void delete(char *name, int flagName)
{
    for (int i = 0; i < flagName; i++) // passa por todos os nomes já criados
    {
        if (strcmp(node.names[i], name) == 0) // procura o nome
        {
            strcpy(node.names[i], "\0");       // apaga o nome
            for (int j = i; j < flagName; j++) // shift left
            {
                strcpy(node.names[j], node.names[j + 1]);
            }
        }
    }
}
void showNames(int flagName)
{
    for (int i = 0; i < flagName; i++)
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
    for (int i = 0; i < maxclits; i++) // maxclits só é incrementado depois de um inverno ser
    {
        n = write(node.vizInt[i].fd, bufsend, l);
        if (n == -1) /*error*/
        {
            printf("error write int tcp.c\n");
            exit(1);
        }
    }
}
void showTopo(int maxclits) // maxclits para o for dos viz internos
{
    printf("Vizinho Externo:\nid:%s\nip:%s\nporto:%s\n", node.vizExt.IDv, node.vizExt.IPv, node.vizExt.Portv);
    for (int i = 0; i < maxclits; i++)
    {
        printf("Vizinho Interno %d:\nid:%s\nip:%s\nporto:%s\n", i, node.vizInt[i].IDv, node.vizInt[i].IPv, node.vizInt[i].Portv);
    }
    printf("Vizinho Backup:\nid:%s\nip:%s\nporto:%s\n", node.vizBackup.IDv, node.vizBackup.IPv, node.vizBackup.Portv);
};