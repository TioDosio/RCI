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
    int flag = 0;
    sprintf(bufsend, "QUERY %s %s %s\n", dest, id, name);
    printf("bufsend:%s\n", bufsend);
    printf("AAAAAAFD:%d\n", node.vizExt.fd);
    for (int i = 0; i < node.maxInter; i++)
    {
        if (strcmp(dest, node.vizInt[i].IDv) == 0)
        {
            write(node.vizInt[i].fd, bufsend, strlen(bufsend));
            flag = 1;
        }
    }
    if (strcmp(dest, node.vizExt.IDv) == 0)
    {
        write(node.vizExt.fd, bufsend, strlen(bufsend));
        flag = 1;
    }
    if (flag == 0)
    {
        write(node.vizExt.fd, bufsend, strlen(bufsend));
        for (int i = 0; i < node.maxInter; i++) // node.maxInter só é incrementado depois de um inverno ser
        {
            write(node.vizInt[i].fd, bufsend, strlen(bufsend));
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
void query(destR, origR, nameR, fdR, id)
{
    char bufsend[100];
    strcpy(bufsend, "");
    int flag = 0;
    printf("QUERY RECEBIDO, %s %s %s\n", destR, origR, nameR);
    if (strcmp(destR, id) == 0) // se o destino for o próprio nó
    {
        for (int i = 0; i <= node.flagName, i++) // procura o name na lista de names
        {
            if (strcmp(node.names[i], nameR) == 0) // se encontrar o name retorna o CONTENT para onde veio o QUERY
            {
                printf("NOME ENCONTRADO\n");
                sprintf(bufsend, "CONTENT %s %s %s\n", origR, destR, nameR);
                write(fdR, bufsend, strlen(bufsend));
                flag = 1;
            }
        }
        if (flag = 0) // se não encontrar o name retorna o NOCONTENT para onde veio o QUERY
        {
            printf("NOME NÃO ENCONTRADO\n");
            sprintf(bufsend, "NOCONTENT %s %s %s\n", origR, destR, nameR);
            write(fdR, bufsend, strlen(bufsend));
        }
    }
    else // Se destino não for o próprio nó
    {
        flag = 0;
        for (int i = 0; i < node.maxInter; i++) // Passa por todos os vizinhos internos
        {
            if (strcmp(destR, node.vizInt[i].IDv) == 0) // vê se algum dos seus vizinhos é o destino
            {
                sprintf(bufsend, "QUERY %s %s %s\n", destR, origR, nameR);
                write(node.vizInt[i].fd, bufsend, strlen(bufsend)); // manda o QUERY para o vizinho interno
                flag = 1;
            }
        }
        if (strcmp(destR, node.vizExt.IDv) == 0) // vê se o vizinho externo é o destino
        {
            sprintf(bufsend, "QUERY %s %s %s\n", destR, origR, nameR);
            write(node.vizExt.fd, bufsend, strlen(bufsend)); // manda o QUERY para o vizinho externo
            flag = 1;
        }
        if (flag == 0) // se nenhum dos seus vizinhos for o destino manda FLOOD
        {
            sprintf(bufsend, "QUERY %s %s %s\n", destR, origR, nameR);
            write(node.vizExt.fd, bufsend, strlen(bufsend)); // envia o QUERY para o vizinho externo
            for (int i = 0; i < node.maxInter; i++)          // envia o QUERY para todos os vizinhos internos
            {
                write(node.vizInt[i].fd, bufsend, strlen(bufsend));
            }
        }
    }
}
void CNContent(CNC, destR, origR, nameR, fdR, id)
{
    if (CNC == 0)
    {
        char bufCNC[10];
        strcpy(bufCNC, "CONTENT");
    }
    else if (CNC == 1)
    {
        char bufCNC[10];
        strcpy(bufCNC, "NOCONTENT");
    }
    char bufsend[100];
    strcpy(bufsend, "");
    if (strcmp(destR, id) != 0) // se o destino for o próprio nó
    {
        flag = 0;
        for (int i = 0; i < node.maxInter; i++) // Passa por todos os vizinhos internos
        {
            if (strcmp(destR, node.vizInt[i].IDv) == 0) // vê se algum dos seus vizinhos é o destino
            {
                sprintf(bufsend, "QUERY %s %s %s\n", destR, origR, nameR);
                write(node.vizInt[i].fd, bufsend, strlen(bufsend)); // manda o QUERY para o vizinho interno
                flag = 1;
            }
        }
        if (strcmp(destR, node.vizExt.IDv) == 0) // vê se o vizinho externo é o destino
        {
            sprintf(bufsend, "QUERY %s %s %s\n", destR, origR, nameR);
            write(node.vizExt.fd, bufsend, strlen(bufsend)); // manda o QUERY para o vizinho externo
            flag = 1;
        }
        if (flag == 0) // se nenhum dos seus vizinhos for o destino manda FLOOD
        {
            sprintf(bufsend, "QUERY %s %s %s\n", destR, origR, nameR);
            write(node.vizExt.fd, bufsend, strlen(bufsend)); // envia o QUERY para o vizinho externo
            for (int i = 0; i < node.maxInter; i++)          // envia o QUERY para todos os vizinhos internos
            {
                write(node.vizInt[i].fd, bufsend, strlen(bufsend));
            }
        }
    }
}