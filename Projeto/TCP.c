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

/*
Faz a ligação TCP entre nós e envia NEW através da ligação estabelicida.
IP-> recebe ip que nos queremos ligar;
TCP-> recebe porto a que nos queremos ligar
*/
void client_tcp(char *IP, char *TCP)
{
    char buffer[128] = "";
    struct addrinfo hints, *res;
    int n;
    node.vizExt.fd = socket(AF_INET, SOCK_STREAM, 0); // criação de um socket TCP
    printf("socket clientTCP:%d\n", node.vizExt.fd);
    if (node.vizExt.fd == -1)
    {
        printf("erro socket tcp.c");
        exit(1); // error
    }
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM;
    n = getaddrinfo(node.vizExt.IPv, node.vizExt.Portv, &hints, &res); // preenche a estrutura res com o endereço do vizinho externo
    if (n != 0)                                                        /*error*/
    {
        printf("erro getaddrinfo tcp.c");
        exit(1);
    }
    n = connect(node.vizExt.fd, res->ai_addr, res->ai_addrlen); // estabelece a ligação TCP
    if (n == -1)                                                /*error*/
    {
        printf("error connect tcp.c\n");
        exit(1);
    }
    sprintf(buffer, "NEW %s %s %s\n", node.id, IP, TCP); // mensagem enviada ao no a que se liga com NEW ID IP PORTO
    printf("enviado por mim: %s\n", buffer);
    write(node.vizExt.fd, buffer, strlen(buffer));
    freeaddrinfo(res);
}
/*
Faz a gestão da ligação através do comando djoin
net->rede do nó que nos queremos ligar;
IP->recebe ip que nos queremos ligar;
TCP->recebe porto a que nos queremos ligar
bootID->recebe id do nó que nos queremos ligar;
bootIP->recebe ip do nó que nos queremos ligar;
bootTCP->recebe porto do nó que nos queremos ligar;
*/
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
        printf("Ip:%s,porto:%s",node.vizExt.IPv,node.vizExt.Portv);
        client_tcp(IP, TCP);
    }
}
/*
Fecha todas as ligações TCP e UDP e limpa as tabelas de expedição e dados dos vizinhos. Retira apenas do servidor de nós se não formos o próprio nó.
net-> recebe rede onde está o nó que queremos retirar do servidor de nós;
IP-> recebe ip que queremos retirar do servidor de nós;
TCP-> recebe porto a que queremos retirar do servidor de nós
*/
void leave(char *net, char *IP, char *TCP)
{
    unreg(net, IP, TCP); // retira do servidor de nós
    for (int i = 0; i < node.maxInter; i++)
    {
        if (node.vizInt[i].fd != -2)
        {
            close(node.vizInt[i].fd); // fecha a ligação TCP com o interno
            node.vizInt[i].fd = -2;   // coloca o fd a -2 para indicar que não tem ligação TCP
        }
    }
    if (node.vizExt.fd != -2)
    {
        close(node.vizExt.fd); // fecha a ligação TCP com o externo
        node.vizExt.fd = -2;   // coloca o fd a -2 para indicar que não tem ligação TCP
    }
    for (int i = 0; i < 100; i++)
    {
        node.tabExp[i] = -2; // limpa a tabela de expedição
    }
    strcpy(node.vizExt.IDv, ""); // limpa os dados do vizinho externo
    strcpy(node.vizExt.IPv, "");
    strcpy(node.vizExt.Portv, "");
    strcpy(node.vizBackup.IDv, ""); // limpa os dados do vizinho de backup
    strcpy(node.vizBackup.IPv, "");
    strcpy(node.vizBackup.Portv, "");
    for (int i = 0; i < node.maxInter; i++)
    {
        strcpy(node.vizInt[i].IDv, ""); // limpa os dados dos vizinhos internos
        strcpy(node.vizInt[i].IPv, "");
        strcpy(node.vizInt[i].Portv, "");
    }
}