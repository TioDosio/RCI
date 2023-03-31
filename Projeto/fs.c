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
extern int debug;
/*
Cria-se um conteudo com o nome dado na variavel.
name->recebe o conteudo que queremos criar;
*/
void create(char *name)
{
    int flag = 0;
    if (strlen(name) > 100)
    {
        printf("O conteudo não pode ser criado, limite de 100 caracteres atingido\n");
        return;
    }
    for (int i = 0; i < node.flagName; i++) // passa por todos os nomes já criados
    {
        if (node.flagName < 50) // verifica se o limite de conteudos foi atingido
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
    if (flag == 0) // se o nome não existir
    {
        strcpy(node.names[node.flagName], name); // cria o nome na posição flagName
        printf("Conteudo criado:%s\n", node.names[node.flagName]);
        node.flagName++;
    }
}
/*
Elimina-se um conteudo com o nome dado na variavel.
name->recebe o conteudo que queremos eliminar;
*/
void delet(char *name)
{
    int flag = 0;
    for (int i = 0; i < node.flagName; i++) // passa por todos os nomes já criados
    {
        if (strcmp(node.names[i], name) == 0) // procura o nome
        {
            strcpy(node.names[i], "\0"); // apaga o nome
            for (int j = i; j < node.flagName; j++)
            {
                strcpy(node.names[j], node.names[j + 1]); // move os nomes para a esquerda
            }
            node.flagName--; // diminui o numero de nomes
            flag = 1;
            printf("O conteudo foi apagado\n");
        }
    }
    if (flag == 0) // se o nome não existir
    {
        printf("O conteudo não existe\n");
    }
}
/*
Mostra os conteudos presentes no nó.
*/
void showNames()
{
    printf("Conteudos do nó:\n");
    for (int i = 0; i < node.flagName; i++)
    {
        printf("Conteudo %d »» %s\n", i, node.names[i]);
    }
}
/*
Pesquisa na rede o conteudo com o nome dado na variavel no nó com o id dado na variavel dest.
dest->recebe o id do nó onde queremos pesquisar o conteudo;
name->recebe o nome que queremos pesquisar;
*/
void get(char *dest, char *name)
{
    char bufsend[100] = "";
    sprintf(bufsend, "QUERY %s %s %s\n", dest, node.id, name);
    if (debug = 1)
    {
        printf("Enviado:%s\n", bufsend);
    }
    if (node.tabExp[atoi(dest)] != -2) // se o destino estiver na tabela de expedição
    {
        if (debug = 1)
        {
            printf("FLOOD\n");
        }
        if (node.tabExp[atoi(dest)] == atoi(node.vizExt.IDv)) // se o destino for o vizinho externo
        {
            write(node.vizExt.fd, bufsend, strlen(bufsend));
        }
        else
        {
            for (int i = 0; i < node.maxInter; i++)
            {
                if (node.tabExp[atoi(dest)] == atoi(node.vizInt[i].IDv)) // se o destino for um vizinho interno
                {
                    write(node.vizInt[i].fd, bufsend, strlen(bufsend));
                }
            }
        }
    }
    else // se o destino não estiver na tabela de expedição
    {
        for (int i = 0; i < node.maxInter; i++) // FLOOD internos
        {
            write(node.vizInt[i].fd, bufsend, strlen(bufsend));
        }
        write(node.vizExt.fd, bufsend, strlen(bufsend)); // FLOOD externo
    }
}
/*
Mostra a tologia da rede.
*/
void showTopo()
{
    printf("Nó=>%s\n", node.id);
    printf("Vizinho Externo:    id:%s ip:%s porto:%s\n", node.vizExt.IDv, node.vizExt.IPv, node.vizExt.Portv);
    for (int i = 0; i < node.maxInter; i++)
    {
        printf("Vizinho Interno %d:     id:%s ip:%s porto:%s\n", i, node.vizInt[i].IDv, node.vizInt[i].IPv, node.vizInt[i].Portv);
    }
    printf("Vizinho Backup:     id:%s ip:%s porto:%s\n", node.vizBackup.IDv, node.vizBackup.IPv, node.vizBackup.Portv);
}
/*
Mostra a tabela de expedição do nó.
*/
void showRouting()
{
    printf("Tabela Expedicao:\n");
    for (int i = 0; i < 100; i++)
    {
        if (node.tabExp[i] != -2) // se o destino estiver na tabela de expedição
        {
            printf("Destino:%d »» Vizinho:%d\n", i, node.tabExp[i]);
        }
    }
}
/*
Recebe o pedido de pesquisa, procura o conteudo em sim ou retransmite a mensagem para os vizinhos.
destR->recebe o id do nó onde queremos pesquisar o conteudo;
origR->recebe o id do nó que enviou o QUERY;
name->recebe o nome que queremos pesquisar;
fdR->recebe o descritor do nó que nos enviou o QUERY;
*/
void query(char *destR, char *origR, char *nameR, int fdR)
{
    char bufsend[120] = "";
    int flag = 0;
    if (debug = 1)
    {
        printf("Query recebido\n");
    }
    if (strcmp(destR, node.id) == 0) // se o destino formos este nó
    {
        for (int i = 0; i < node.flagName; i++) // procura o conteudo na lista de conteudos
        {
            if (strcmp(node.names[i], nameR) == 0) // se encontrar o conteudo retorna o CONTENT para onde veio o QUERY
            {
                printf("Conteudo Encontrado\n");
                sprintf(bufsend, "CONTENT %s %s %s\n", origR, destR, nameR);
                write(fdR, bufsend, strlen(bufsend)); // envia o CONTENT para o nó que nos enviou o QUERY
                flag = 1;
                break;
            }
        }
        if (flag == 0) // se não encontrar o name retorna o NOCONTENT para onde veio o QUERY
        {
            printf("Conteudo Não Encontrado\n");
            sprintf(bufsend, "NOCONTENT %s %s %s\n", origR, destR, nameR);
            write(fdR, bufsend, strlen(bufsend));
        }
    }
    else // Se o destino não for o próprio nó
    {
        if (debug = 1)
        {
            printf("Retransmite o Query\n");
        }
        sprintf(bufsend, "QUERY %s %s %s\n", destR, origR, nameR);
        if (node.tabExp[atoi(destR)] == -2) // O destino na Tabela não está preenchido então damos FLOOD
        {
            if (node.vizExt.fd != fdR) // Se o vizinho externo não for o nó que nos enviou o QUERY
            {
                write(node.vizExt.fd, bufsend, strlen(bufsend)); // envia o QUERY para o vizinho externo
            }
            for (int i = 0; i < node.maxInter; i++)
            {
                if (node.vizInt[i].fd != fdR) // Se o vizinho interno não for o nó que nos enviou o QUERY
                {
                    write(node.vizInt[i].fd, bufsend, strlen(bufsend)); // envia o QUERY para todos os vizinho interno
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
                if (node.tabExp[atoi(destR)] == atoi(node.vizInt[i].IDv)) // Se o destino estiver na tabela de expedição e for um vizinho interno
                {
                    write(node.vizInt[i].fd, bufsend, strlen(bufsend)); // envia o QUERY para o vizinho interno
                }
            }
        }
    }
}
/*
Pesquisa na rede o conteudo com o nome dado na variavel no nó com o id dado na variavel dest.
CNC->recebe 0 se o conteudo for encontrado e 1 se não for encontrado;
destR->recebe o id do nó onde a mensagem CONTENT ou NOCONTENT tem de ser entregue;
origR->recebe o id do nó que enviou o CONTENT ou NOCONTENT em primeiro lugar;
nameR->recebe o conteudo da pesquisa feita;
fdR->recebe o descritor do nó que nos enviou o CONTENT ou NOCONTENT;
*/
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
        printf("%s\n", bufCNC);
    }
    else // Se destino não for o próprio nó
    {
        if (debug = 1)
        {
            printf("Retransmite %s\n", bufCNC);
        }
        sprintf(bufcont, "%s %s %s %s\n", bufCNC, destR, origR, nameR);
        if (node.tabExp[atoi(destR)] == atoi(node.vizExt.IDv)) // Se o destino estiver na tabela de expedição e for o vizinho externo
        {
            write(node.vizExt.fd, bufcont, strlen(bufcont));
        }
        else
        {
            for (int i = 0; i < node.maxInter; i++)
            {
                if (node.tabExp[atoi(destR)] == atoi(node.vizInt[i].IDv)) // Se o destino estiver na tabela de expedição e for um vizinho interno
                {
                    write(node.vizInt[i].fd, bufcont, strlen(bufcont));
                    break;
                }
            }
        }
    }
}
/*
Recebe a mensagem WITHDRAW e retira o nó da variavel idR da tabela de expedição.
idR->recebe o id do nó que queres retirar da tabela de expedição;
fdR->recebe o descritor do nó que nos enviou o WITHDRAW;
*/
void wdraw(char *idR, int fdR)
{
    if (debug = 1)
    {
        printf("Withdraw %s\n", idR);
    }
    char bufsend[13] = "";        // WITHDRAW + id + \n
    node.tabExp[atoi(idR)] = -2;  // retira o destino da tabela de expedição
    for (int i = 0; i < 100; i++) // Se um vizinho der leave nós tiramos esse caminho da tabela de expedição
    {
        if (node.tabExp[i] == atoi(idR))
        {
            node.tabExp[i] = -2; // retira o vizinho da tabela de expedição
        }
    }
    sprintf(bufsend, "WITHDRAW %s\n", idR);
    for (int i = 0; i < node.maxInter; i++) // FLOOD internos
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