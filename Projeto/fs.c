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
    node.flagName++;
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
            node.flagName--;
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
void get(char *dest, char *name)
{
    // QUERY dest orig name
    char bufsend[100];
    sprintf(bufsend, "QUERY %s %s %s\n", dest, node.id, name);
    printf("bufsend:%s\n", bufsend);
    if (node.tabExp[atoi(dest)] != -2) // se o destino estiver na tabela de exp
    {
        if (node.tabExp[atoi(dest)] == atoi(node.vizExt.IDv))
        {
            write(node.vizExt.fd, bufsend, strlen(bufsend));
        }
        for (int i = 0; i < node.maxInter; i++)
        {
            if (node.tabExp[atoi(dest)] == atoi(node.vizInt[i].IDv))
            {
                write(node.vizInt[i].fd, bufsend, strlen(bufsend));
            }
        }
    }
    else
    {                                           // FLOOD
        for (int i = 0; i < node.maxInter; i++) // FLOOD internos
        {
            write(node.vizInt[i].fd, bufsend, strlen(bufsend)); // FLOOD internos
        }
        write(node.vizExt.fd, bufsend, strlen(bufsend)); // FLOOD externo
    }
}
void showTopo() // node.maxInter para o for dos viz internos
{
    printf("Vizinho Externo:    id:%s ip:%s porto:%s fd:%d\n", node.vizExt.IDv, node.vizExt.IPv, node.vizExt.Portv, node.vizExt.fd);
    for (int i = 0; i < node.maxInter; i++)
    {
        printf("Vizinho Interno %d:     id:%s ip:%s porto:%s fd:%d\n", i, node.vizInt[i].IDv, node.vizInt[i].IPv, node.vizInt[i].Portv, node.vizInt[i].fd);
    }
    printf("Vizinho Backup:     id:%s ip:%s porto:%s\n", node.vizBackup.IDv, node.vizBackup.IPv, node.vizBackup.Portv);
}
void showRouting()
{
    printf("Routing table:\n");
    for (int i = 0; i < 100; i++)
    {
        if (node.tabExp[i] != -2)
        {
            printf("TabelaExp[%d]:%d\n", i, node.tabExp[i]);
        }
    }
}
void query(char *destR, char *origR, char *nameR, int fdR)
{
    char bufsend[100];
    strcpy(bufsend, "");
    int flag = 0;
    printf("QUERY RECEBIDO, %s %s %s\n", destR, origR, nameR);
    if (strcmp(destR, node.id) == 0) // se o destino for o próprio nó
    {
        for (int i = 0; i < node.flagName; i++) // procura o name na lista de names
        {
            if (strcmp(node.names[i], nameR) == 0) // se encontrar o name retorna o CONTENT para onde veio o QUERY
            {
                printf("NOME ENCONTRADO\n");
                sprintf(bufsend, "CONTENT %s %s %s\n", origR, destR, nameR);
                write(fdR, bufsend, strlen(bufsend));
                flag = 1;
                break;
            }
        }
        if (flag == 0) // se não encontrar o name retorna o NOCONTENT para onde veio o QUERY
        {
            printf("NOME NÃO ENCONTRADO\n");
            sprintf(bufsend, "NOCONTENT %s %s %s\n", origR, destR, nameR);
            write(fdR, bufsend, strlen(bufsend));
        }
    }
    else // Se destino não for o próprio nó
    {
        sprintf(bufsend, "QUERY %s %s %s\n", destR, origR, nameR);
        if (node.tabExp[atoi(destR)] != -2) // a tabela de Expedição está preenchida com alguma coisa
        {
            write(fdR, bufsend, strlen(bufsend)); // envia o QUERY para o vizinho que está na tabela de expedição
        }
        else // Se a tabela de expedição não estiver preenchida
        {
            for (int i = 0; i < node.maxInter; i++) // node.maxInter só é incrementado depois de um inverno ser
            {
                if (node.vizInt[i].fd != fdR)
                {
                    write(node.vizInt[i].fd, bufsend, strlen(bufsend)); // FLOOD internos
                }
            }
            if (node.vizExt.fd != fdR)
            {
                write(node.vizExt.fd, bufsend, strlen(bufsend)); // FLOOD externo
            }
        }
    }
}
void CNContent(int CNC, char *destR, char *origR, char *nameR, int fdR)
{
    char bufCNC[10], bufsend[100];
    strcpy(bufsend, "");
    if (CNC == 0)
    {
        strcpy(bufCNC, "CONTENT");
    }
    else if (CNC == 1)
    {
        strcpy(bufCNC, "NOCONTENT");
    }
    if (strcmp(destR, node.id) != 0) // se o destino for o próprio nó
    {
        printf("%s VOLTOU", bufCNC);
    }
    else // Se destino não for o próprio nó
    {
        sprintf(bufsend, "%s %s %s %s\n", bufCNC, destR, origR, nameR);
        for (int i = 0; i < node.maxInter; i++)
        {
            if (node.tabExp[atoi(destR)] == atoi(node.vizInt[i].IDv))
            {
                if (node.vizInt[i].fd != fdR)
                {
                    write(node.vizInt[i].fd, bufsend, strlen(bufsend)); // FLOOD internos
                }
            }
        }
        if (node.tabExp[atoi(destR)] == atoi(node.vizExt.IDv))
        {
            if (node.vizExt.fd != fdR)
            {
                write(node.vizExt.fd, bufsend, strlen(bufsend)); // FLOOD internos
            }
        }
    }
}