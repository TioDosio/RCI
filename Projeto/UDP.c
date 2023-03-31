#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <time.h>
#include "UDP.h"
#include "TCP.h"
#include "fs.h"
extern struct NO node; // ter variavel global em varios ficheiros

/*
Função que regista um nó no servidor de nós e faz a gestão caso já haja um nó com o mesmo id.
net-> rede do nó que queremos registar no servidor de nós;
IP-> ip do nó que queremos registar no servidor de nós;
TCP-> porto do nó que queremos registar no servidor de nós;
*/
void reg(char *net, char *IP, char *TCP)
{
    char sendV[50];
    int F = show(1, net); // F recebe o id do nó que vamos usar para nos registar
    if (F >= 0 && F <= 9) // se o id for menor que 10 acrescenta um 0 atrás para depois ficar de acordo com o protocolo
    {
        strcpy(node.id, "");
        sprintf(node.id, "0%d", F);
        printf("id: %s\n", node.id);
    }
    else if (F >= 10 && F <= 99)
    {
        strcpy(node.id, "");
        sprintf(node.id, "%d", F);
        printf("id: %s\n", node.id);
    }
    strcpy(node.vizBackup.IDv, node.id); // inicializa o vizinho de backup com a própria informação
    strcpy(node.vizBackup.IPv, IP);
    strcpy(node.vizBackup.Portv, TCP);
    sprintf(sendV, "REG %s %s %s %s", net, node.id, IP, TCP); // REG net id IP TCP
    struct addrinfo hints, *res;
    int fd, errcode;
    ssize_t n;
    struct sockaddr addr;
    char buffOKs[8]; // max = OKUNREG
    socklen_t addrlen;
    fd = socket(AF_INET, SOCK_DGRAM, 0); // UDP socket
    if (fd == -1)                        /*error*/
        exit(1);
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;                                               // IPv4
    hints.ai_socktype = SOCK_DGRAM;                                          // UDP socket
    errcode = getaddrinfo("tejo.tecnico.ulisboa.pt", "59000", &hints, &res); // coloca o endereço do servidor de nós na estrutura res
    if (errcode != 0)                                                        /*error*/
    {
        printf("erro get addrinfo reg.c");
        exit(1);
    }
    int i;
    for (i = 0; i < 3; i++) // tenta esperar por uma resposta 3 vezes caso o servidor não esteja disponivel sai do programa
    {
        struct timeval time;
        time.tv_sec = 3; // tempo de espera 3 segundos
        time.tv_usec = 0;
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(fd, &fds);
        n = sendto(fd, sendV, strlen(sendV), 0, res->ai_addr, res->ai_addrlen); // Envia o registo para o servidor de nós
        if (n == -1)
        {
            printf("erro send reg.c");
            exit(1);
        }
        int flagTempo = select(fd + 1, &fds, NULL, NULL, &time); // Espera por uma resposta do servidor de nós
        if (flagTempo == -1)
        {
            printf("Erro no select2\n");
            return;
        }
        else if (FD_ISSET(fd, &fds))
        {
            addrlen = sizeof(addr);
            n = recvfrom(fd, buffOKs, sizeof(buffOKs), 0, &addr, &addrlen); // Recebe a resposta do servidor
            if (n == -1)
            {
                printf("erro recv reg.c");
                exit(1);
            }
            FD_CLR(fd, &fds);
            FD_ZERO(&fds);
            break;
        }
    }
    if (i == 3)
    {
        printf("Servidor indisponivel\n");
        exit(1);
    }

    buffOKs[n] = '\0'; // adiciona terminador de string
    printf("%s\n", buffOKs);
    if (node.flagVaz > 0) // se não formos o único nó na rede fazemos connect a um nó que já esteja na rede
    {
        client_tcp(IP, TCP); // fazemos connect ao nó que nos quer registar
    }
    freeaddrinfo(res);
    close(fd);
}

/*
Retira do servidor de nós o nó pretendido.
net-> rede do nó que queremos retirar do servidor de nós;
IP-> ip do nó que queremos retirar do servidor de nós;
TCP-> porto so nó que queremos retirar do servidor de nós;
*/
void unreg(char *net, char *IP, char *TCP)
{
    char sendV[50], bufOKs[10];
    sprintf(sendV, "UNREG %s %s", net, node.id); // NODES net
    printf("sending: %s\n", sendV);
    struct addrinfo hints, *res;
    int fd, errcode;
    ssize_t n;
    struct sockaddr addr;
    socklen_t addrlen;
    fd = socket(AF_INET, SOCK_DGRAM, 0); // UDP socket
    if (fd == -1)                        /*error*/
    {
        printf("erro socket leave.c\n");
        exit(1);
    }
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;                                               // IPv4
    hints.ai_socktype = SOCK_DGRAM;                                          // UDP socket
    errcode = getaddrinfo("tejo.tecnico.ulisboa.pt", "59000", &hints, &res); // coloca o endereço do servidor de nós na estrutura res
    if (errcode != 0)                                                        /*error*/
    {
        printf("erro get addrinfo leave.c\n");
        exit(1);
    }
    int i;
    for (i = 0; i < 3; i++) // tenta esperar por uma resposta 3 vezes caso o servidor não esteja disponivel sai do programa
    {
        struct timeval time;
        time.tv_sec = 3; // tempo de espera 3 segundos
        time.tv_usec = 0;
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(fd, &fds);
        n = sendto(fd, sendV, strlen(sendV), 0, res->ai_addr, res->ai_addrlen); // Envia o pedido de remoção para o servidor de nós
        if (n == -1)
        {
            printf("erro send reg.c");
            exit(1);
        }
        int flagTempo = select(fd + 1, &fds, NULL, NULL, &time); // Espera por uma resposta do servidor de nós
        if (flagTempo == -1)
        {
            printf("Erro no select2\n");
            return;
        }
        else if (FD_ISSET(fd, &fds))
        {
            addrlen = sizeof(addr);
            n = recvfrom(fd, bufOKs, sizeof(bufOKs), 0, &addr, &addrlen); // Recebe a resposta do servidor
            if (n == -1)
            {
                printf("erro recv reg.c");
                exit(1);
            }
            FD_CLR(fd, &fds);
            FD_ZERO(&fds);
            break;
        }
    }
    if (i == 3)
    {
        printf("Servidor indisponivel\n");
        exit(1);
    }
    bufOKs[n] = '\0'; // adiciona terminador de string
    printf("%s\n", bufOKs);
    close(fd);
    freeaddrinfo(res);
}

/*
Pede ao servidor de nós a lista de nós da rede pretendida. Com a flag a 1 também faz a escolha do nó que queremos ligar.
falgsS-> flag que indica se queremos ver a lista de nós da rede ou também fazer a escolha do que nos vamos ligar;
net-> rede que queremos saber informações no servidor de nós;
*/
int show(int flagS, char *net)
{
    char sendV[50];
    sprintf(sendV, "NODES %s", net); // NODES net
    struct addrinfo hints, *res;
    int fd, errcode;
    ssize_t n;
    struct sockaddr addr;
    char buffer[2500]; // buffer para receber a resposta do servidor de nós
    strcpy(buffer, "");
    socklen_t addrlen;
    fd = socket(AF_INET, SOCK_DGRAM, 0); // UDP socket
    if (fd == -1)                        /*error*/
    {
        printf("erro socket show\n");
        exit(1);
    }
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;      // IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP socket
    errcode = getaddrinfo("tejo.tecnico.ulisboa.pt", "59000", &hints, &res);
    if (errcode != 0) /*error*/
    {
        printf("erro get addrinfo show\n");
        exit(1);
    }
    int i;
    for (i = 0; i < 3; i++) // tenta esperar por uma resposta 3 vezes caso o servidor não esteja disponivel sai do programa
    {
        struct timeval time;
        time.tv_sec = 3; // tempo de espera 3 segundos
        time.tv_usec = 0;
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(fd, &fds);
        n = sendto(fd, sendV, strlen(sendV), 0, res->ai_addr, res->ai_addrlen); // Envia o pedido de informação para o servidor de nós
        if (n == -1)
        {
            printf("erro send reg.c");
            exit(1);
        }
        int flagTempo = select(fd + 1, &fds, NULL, NULL, &time); // Espera por uma resposta do servidor de nós
        if (flagTempo == -1)
        {
            printf("Erro no select2\n");
        }
        else if (FD_ISSET(fd, &fds))
        {
            addrlen = sizeof(addr);
            n = recvfrom(fd, buffer, sizeof(buffer), 0, &addr, &addrlen); // Recebe a resposta do servidor
            if (n == -1)
            {
                printf("erro recv reg.c");
                exit(1);
            }
            FD_CLR(fd, &fds);
            FD_ZERO(&fds);
            break;
        }
    }
    if (i == 3)
    {
        printf("Servidor indisponivel\n");
        exit(1);
    }
    buffer[n] = '\0'; // adiciona terminador de string
    printf("%s\n\n", buffer);
    int intIDv, intID;
    node.flagVaz = 0;
    if (flagS == 1) // so entra se for chamada com o join
    {
        intID = atoi(node.id); // converte o id do nó para inteiro
        char *saveptr, IDv[100][11], IPv[100][20], Portv[100][6];
        char *line = strtok_r(buffer, "\n", &saveptr); // separa a string em linhas
        line = strtok_r(NULL, "\n", &saveptr);         // ignora a primeira linha
        while (line != NULL)
        {
            sscanf(line, "%s %s %s", IDv[node.flagVaz], IPv[node.flagVaz], Portv[node.flagVaz]); // guarda nos arrays os valores de id, ip e porto
            intIDv = atoi(IDv[node.flagVaz]);                                                    // converte o id do nó para inteiro
            while (intID == intIDv)                                                              // se o id do nó for igual ao do nó que queremos ligar
            {
                intID = rand() % 100; // gera um novo id aleatorio
            }
            node.flagVaz++; // conta o numero de nós na rede
            line = strtok_r(NULL, "\n", &saveptr);
        }
        if (line == NULL && node.flagVaz == 0) // se a rede estiver vazia
        {
            printf("Sou o primeiro a entrar na rede\n");
        }
        if (node.flagVaz > 0) // se a rede nao estiver vazia
        {
            int r = rand() % (node.flagVaz); // escolhe um nó aleatorio da rede
            strcpy(node.vizExt.IDv, IDv[r]); // guarda os valores do nó escolhido
            strcpy(node.vizExt.IPv, IPv[r]);
            strcpy(node.vizExt.Portv, Portv[r]);
        }
        else
        {
            intID = 111; // se a rede estiver vazia o id do nó é 111
        }
    }
    strcpy(buffer, "");
    close(fd);
    freeaddrinfo(res);
    return intID; // retorna o id do nó
}