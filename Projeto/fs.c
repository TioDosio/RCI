#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "UDP.h"
#include "TCP.h"
#include "fs.h"
extern struct NO node;

void create(char *name)
{
    int flag = 0;
    for (int i = 0; i < node.flagName; i++) // passa por todos os nomes já criados
    {
        if (node.flagName < 50)
        {
            if (strcmp(node.names[i], name) == 0) // procura o nome
            {
                printf("O conteudo já existe\n");
                flag = 1;
            }
        }
        else
        {
            printf("O conteudo não pode ser criado, limite de 50 conteudos atingido\n");
            flag = 1;
        }
    }
    if (flag == 0)
    {
        strcpy(node.names[node.flagName], name);
        printf("Posição:%d, name:%s\n", node.flagName, node.names[node.flagName]);
        node.flagName++;
    }
}
void delet(char *name)
{
    int flag = 0;
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
            flag = 1;
            printf("O conteudo foi apagado\n");
        }
    }
    if (flag == 0)
    {
        printf("O conteudo não existe\n");
    }
}
void showNames()
{
    printf("Conteudos do nó:\n");
    for (int i = 0; i < node.flagName; i++)
    {
        printf("Conteudo %d »» %s\n", i, node.names[i]);
    }
}
void get(char *dest, char *name)
{
    // QUERY dest orig name
    char bufsend[100] = "";
    sprintf(bufsend, "QUERY %s %s %s\n", dest, node.id, name);
    if (node.tabExp[atoi(dest)] != -2) // se o destino estiver na tabela de exp
    {
        if (node.tabExp[atoi(dest)] == atoi(node.vizExt.IDv))
        {
            write(node.vizExt.fd, bufsend, strlen(bufsend));
        }
        else
        {
            for (int i = 0; i < node.maxInter; i++)
            {
                if (node.tabExp[atoi(dest)] == atoi(node.vizInt[i].IDv))
                {
                    write(node.vizInt[i].fd, bufsend, strlen(bufsend));
                }
            }
        }
    }
    else // FLOOD
    {
        for (int i = 0; i < node.maxInter; i++) // FLOOD internos
        {
            write(node.vizInt[i].fd, bufsend, strlen(bufsend));
            printf("FLOODINT %d\n", i);
        }
        write(node.vizExt.fd, bufsend, strlen(bufsend)); // FLOOD externo
    }
}
void showTopo() // node.maxInter para o for dos viz internos
{
    printf("Nó=>%s\n", node.id);
    printf("Vizinho Externo:    id:%s ip:%s porto:%s\n", node.vizExt.IDv, node.vizExt.IPv, node.vizExt.Portv);
    for (int i = 0; i < node.maxInter; i++)
    {
        printf("Vizinho Interno %d:     id:%s ip:%s porto:%s\n", i, node.vizInt[i].IDv, node.vizInt[i].IPv, node.vizInt[i].Portv);
    }
    printf("Vizinho Backup:     id:%s ip:%s porto:%s\n", node.vizBackup.IDv, node.vizBackup.IPv, node.vizBackup.Portv);
}
void showRouting()
{
    printf("Tabela Expedicao:\n");
    for (int i = 0; i < 100; i++)
    {
        if (node.tabExp[i] != -2)
        {
            printf("Destino:%d »» Vizinho:%d\n", i, node.tabExp[i]);
        }
    }
}
void query(char *destR, char *origR, char *nameR, int fdR)
{
    char bufsend[120] = ""; // NOCONTENT 00 00 name
    int flag = 0;
    printf("QUERY RECEBIDO\n");
    if (strcmp(destR, node.id) == 0) // se o destino formos nós
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
    else // Se o destino não for o próprio nó
    {
        sprintf(bufsend, "QUERY %s %s %s\n", destR, origR, nameR);
        if (node.tabExp[atoi(destR)] == -2) // O destino na Tabela não está preenchido então damos FLOOD
        {
            if (node.vizExt.fd != fdR)
            {
                write(node.vizExt.fd, bufsend, strlen(bufsend));
            }
            for (int i = 0; i < node.maxInter; i++)
            {
                if (node.vizInt[i].fd != fdR)
                {
                    write(node.vizInt[i].fd, bufsend, strlen(bufsend));
                }
            }
        }
        else if (node.tabExp[atoi(destR)] == atoi(node.vizExt.IDv)) // Se o destino estiver na tabela de expedição e for o vizinho externo
        {
            write(node.vizExt.fd, bufsend, strlen(bufsend)); // envia o QUERY para o vizinho externo
        }
        else
        {
            for (int i = 0; i < node.maxInter; i++)
            {
                if (node.tabExp[atoi(destR)] == atoi(node.vizInt[i].IDv))
                {
                    write(node.vizInt[i].fd, bufsend, strlen(bufsend));
                }
            }
        }
    }
}
void CNContent(int CNC, char *destR, char *origR, char *nameR, int fdR)
{
    char bufCNC[10] = "", bufcont[100] = "";
    if (CNC == 0)
    {
        strcpy(bufCNC, "CONTENT");
    }
    else if (CNC == 1)
    {
        strcpy(bufCNC, "NOCONTENT");
    }
    if (strcmp(destR, node.id) == 0) // se o destino for o próprio nó
    {
        printf("--------->%s\n", bufCNC);
    }
    else // Se destino não for o próprio nó
    {
        printf("O dest não sou eu\n");
        sprintf(bufcont, "%s %s %s %s\n", bufCNC, destR, origR, nameR);
        printf("ENVIADO bufcont:%s\n", bufcont);
        if (node.tabExp[atoi(destR)] == atoi(node.vizExt.IDv))
        {
            write(node.vizExt.fd, bufcont, strlen(bufcont));
            printf("EXT-Tenho na tabela de Exp e envio:%s\n", bufcont);
        }
        else
        {
            for (int i = 0; i < node.maxInter; i++)
            {
                if (node.tabExp[atoi(destR)] == atoi(node.vizInt[i].IDv))
                {
                    write(node.vizInt[i].fd, bufcont, strlen(bufcont));
                    printf("INT-Tenho na tabela de Exp e envio:%s\n", bufcont);
                    break;
                }
            }
        }
    }
}
void wdraw(char *idR, int fdR)
{
    char bufsend[13] = "";        // WITHDRAW + id + \n
    node.tabExp[atoi(idR)] = -2;  // retira o destino da tabela de expedição
    for (int i = 0; i < 100; i++) // Se um vizinho der leave nós tiramos esse caminho da tabela de expedição
    {
        if (node.tabExp[i] == atoi(idR))
        {
            node.tabExp[i] = -2;
        }
    }
    sprintf(bufsend, "WITHDRAW %s\n", idR);
    for (int i = 0; i < node.maxInter; i++)
    {
        if (node.vizInt[i].fd != fdR) // Todos os vizinhos internos menos o que enviou o WITHDRAW
        {
            write(node.vizInt[i].fd, bufsend, strlen(bufsend)); // FLOOD internos
        }
    }
    if (node.vizExt.fd != fdR) // Vizinho externo menos se foi ele que nos enviouo WITHDRAW
    {
        write(node.vizExt.fd, bufsend, strlen(bufsend)); // FLOOD externo
    }
}