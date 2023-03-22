/* Grupo 47 André Teodósio e Henrique Delfino.*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include "UDP.h"
#include "TCP.h"
#include "fs.h"

struct NO node;
int maxclits = 0; // Mudar
void help()
{
    printf("Usage: ./cot <IP> <TCP> <regIP> <regUDP>\n");
    printf("Usage: ./cot <IP> <TCP>\n");
    printf("Usage: ./cot\n");
    exit(1);
}
void sr()
{
    printf("show routing\n");
}

int main(int argc, char *argv[])
{
    char IP[20];     // IP do TCP que é dado
    char TCP[20];    // Porto do TCP que é dado
    char regIP[20];  // IP do UDP pode ou não ser dado
    char regUDP[20]; // Porto do UDP que pode ou não ser dado
    int flagName = 0;
    switch (argc) // leitura dos argumentos de entrada
    {
    case 1:
        printf("Sem argumentos\n"); // Não tem argumentos
        exit(1);
    case 3: // Recebe apenas o IP e o TCP
        strcpy(regIP, "193.136.138.142");
        strcpy(regUDP, "59000");
        strcpy(IP, argv[1]); //"127.0.0.1"; ou "127.0.1.1";
        strcpy(TCP, argv[2]);
        printf("IP: %s\n", IP);
        printf("TCP: %s\n", TCP);
        break;
    case 5: // Recebe o IP e o TCP e o IP e o UDP
        printf("Com 4 argumentos\n");
        strcpy(IP, argv[1]); //"127.0.0.1"; ou "127.0.1.1";
        strcpy(TCP, argv[2]);
        strcpy(regIP, argv[3]);
        strcpy(regUDP, argv[4]);
        printf("IP: %s\n", IP);
        printf("TCP: %s\n", TCP);
        break;
    default:
        printf("Argumentos invalidos\n");
        help();
    }
    struct addrinfo hints, *res;
    int errcode, server_fd = -1, n; /*maxclits->counter VizInternos*/ /*server_fd->serverFD*/
    struct sockaddr addr;
    socklen_t addrlen;
    char bufstdin[100]; // ver melhor o tamanho do buffer
    fd_set rfds;        // create file descriptors set
    // create TCP server
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        printf("erro socket main.c");
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;       // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP socket
    hints.ai_flags = AI_PASSIVE;
    if ((errcode = getaddrinfo(NULL, TCP, &hints, &res)) != 0)
    {
        printf("erro get addrinfo main.c");
        exit(1);
    }
    if (bind(server_fd, res->ai_addr, res->ai_addrlen) == -1) // conecta o socket ao endereço
    {
        printf("erro bind main.c");
        exit(1);
    }
    if (listen(server_fd, 5) == -1) // 5 é o numero de conexões que podem estar em espera
    {
        printf("erro get addrinfo main.c");
        exit(1);
    }
    char strV[20], net[20], id[20];
    for (int i = 0; i < 98; i++)
    {
        node.vizInt[i].fd = -1;
    }
    while (1)
    {
        FD_ZERO(&rfds);
        FD_SET(STDIN_FILENO, &rfds);
        FD_SET(server_fd, &rfds);
        int max_fd = (STDIN_FILENO > server_fd) ? STDIN_FILENO : server_fd;
        int Nsel = 1;
        for (int i = 0; i < 98; i++)
        {
            if (node.vizInt[i].fd > 0)
            {
                printf("fdset:%d", i);
                FD_SET(node.vizInt[i].fd, &rfds);
                max_fd = node.vizInt[i].fd;
            }
        }
        if ((Nsel = select(max_fd + 1, &rfds, NULL, NULL, NULL)) < 0)
        {
            printf("erro select main.c");
            perror("error select");
            exit(EXIT_FAILURE);
        }
        for (int ns = 0; ns < Nsel; ns++)
        {
            // check if stdin is ready for reading
            if (FD_ISSET(STDIN_FILENO, &rfds))
            {
                printf("stdin isSET\n");
                fgets(bufstdin, 100, stdin);
                printf("User input: %s", bufstdin);
                sscanf(bufstdin, "%s", strV);
                if (strcmp(strV, "join") == 0) // join net id
                {
                    sscanf(bufstdin, "%s %s %s", strV, net, id);
                    reg(net, id, IP, TCP);
                }
                else if (strcmp(strV, "djoin") == 0) // djoin net id bootid bootIP bootTCP
                {
                    char bootid[3], bootIP[16], bootTCP[5];
                    sscanf(bufstdin, "%s %s %s %s %s %s", strV, net, id, bootid, bootIP, bootTCP);
                    djoin(net, id, IP, TCP, bootid, bootIP, bootTCP);
                }
                else if (strcmp(strV, "create") == 0) // create name
                {
                    char name[101];
                    sscanf(bufstdin, "%s %s", strV, name);
                    create(name, flagName);
                    flagName++;
                }
                else if (strcmp(strV, "delete") == 0) // delete name
                {
                    char name[101];
                    sscanf(bufstdin, "%s %s", strV, name);
                    delete (name, flagName);
                    flagName--;
                }
                else if (strcmp(strV, "get") == 0) // get dest name
                {
                    char dest[4], name[101];
                    sscanf(bufstdin, "%s %s %s", strV, dest, name); // falta mandar a mensagem para os outros nós
                    get(dest, name);
                }
                else if ((strcmp(strV, "show topology") == 0) || (strcmp(strV, "st") == 0)) // show topology (st)
                {
                    showTopo(maxclits);
                }
                else if ((strcmp(strV, "show names") == 0) || (strcmp(strV, "sn") == 0)) // show names (sn)
                {
                    showNames(flagName);
                }
                else if ((strcmp(strV, "show routing") == 0) || (strcmp(strV, "sr") == 0)) // show routing (sr)
                {
                    sr(); // nao feito
                }
                else if (strcmp(strV, "leave") == 0) // leave net id
                {
                    sscanf(bufstdin, "%s %s %s", strV, net, id);
                    leave(net, id, IP, TCP, maxclits);
                }
                else if (strcmp(strV, "show") == 0) // exit
                {
                    sscanf(bufstdin, "%s %s %s", strV, net, id);
                    show(0, net, id, IP, TCP);
                }
                else if (strcmp(strV, "exit") == 0) // exit
                {
                    printf("See yaa soon...\n");
                    exit(0);
                }
                else if (strcmp(strV, "banana") == 0)
                {
                    printf("ID:%s FD:%d", node.vizExt.IDv, node.vizExt.fd);
                    for (int i = 0; i < 100; i++)
                    {
                        printf("ID: %s FD: %d\n", node.vizInt[i].IDv, node.vizInt[i].fd);
                    }
                }
                else
                {
                    printf("Invalid command\n");
                }
                FD_CLR(STDIN_FILENO, &rfds); // fechar fd do stdin
            }
            for (int i = 0; i < maxclits; i++) // maxfd?
            {
                if (FD_ISSET(node.vizInt[i].fd, &rfds))
                {
                    char conv[100];
                    int n = -1;
                    n = read(node.vizInt[i].fd, conv, 100);
                    if (n == -1)
                    {
                        printf("Error read conversa");
                        exit(1);
                    }
                    printf("O que recebe do que saiu:%s\n", conv);
                    if (strcmp(conv, "0") == 0)
                    {
                        printf("Avisou que saiu (0)\n");
                    }
                }
                FD_CLR(node.vizInt[i].fd, &rfds);
            }
            if (FD_ISSET(server_fd, &rfds))
            {
                printf("SERVER IS_SET\n");
                ssize_t g;
                addrlen = sizeof(addr);
                char bufRead[100], cmd[10], bufsend[100] = "";
                if (node.flagVaz == 1) // apenas 2 nós
                {
                    printf("Entra no VAZIO\n");
                    node.vizExt.fd = accept(server_fd, &addr, &addrlen);
                    if ((node.vizExt.fd == -1))
                    {
                        printf("EXT erro accept main.c");
                        exit(1);
                    }
                    n = read(node.vizExt.fd, bufRead, 100); //  vai ser 0 quando o cliente fechar a ligação
                    printf("VAZIO: %s\n", bufRead);
                    sscanf(bufRead, "%s %s %s %s", cmd, node.vizExt.IDv, node.vizExt.IPv, node.vizExt.Portv);
                }
                else // mais de 2 nós
                {
                    printf("Entra no NAO VAZIO\n");
                    node.vizInt[maxclits].fd = accept(server_fd, &addr, &addrlen);
                    if (node.vizInt[maxclits].fd == -1)
                    {
                        printf("INT erro accept main.c");
                        exit(1);
                    }
                    n = read(node.vizInt[maxclits].fd, bufRead, 100);
                    printf("NOT VAZIO: %s\n", bufRead);
                    sscanf(bufRead, "%s %s %s %s", cmd, node.vizInt[maxclits].IDv, node.vizInt[maxclits].IPv, node.vizInt[maxclits].Portv);
                }
                if (n == -1)
                {
                    printf("erro read main.c");
                    exit(1);
                }
                if (strcmp(cmd, "NEW") == 0) // se alguém se ligar ao server
                {
                    if (node.flagVaz == 1) // Apenas 2 nós na rede
                    {
                        sscanf(bufRead, "%s %s %s %s", cmd, node.vizExt.IDv, node.vizExt.IPv, node.vizExt.Portv);
                        g = sprintf(bufsend, "EXTERN %s %s %s\n", node.vizExt.IDv, node.vizExt.IPv, node.vizExt.Portv);
                        n = write(node.vizExt.fd, bufsend, g);
                        if (n == -1)
                        {
                            printf("erro write main.c");
                            exit(1);
                        }
                        printf("enviado ao cliente: %s\n", bufsend);
                    }
                    else // + de 2 nós na rede
                    {
                        sscanf(bufRead, "%s %s %s %s", cmd, node.vizInt[maxclits].IDv, node.vizInt[maxclits].IPv, node.vizInt[maxclits].Portv);
                        g = sprintf(bufsend, "EXTERN %s %s %s\n", node.vizExt.IDv, node.vizExt.IPv, node.vizExt.Portv);
                        n = write(node.vizInt[maxclits].fd, bufsend, g);
                        if (n == -1)
                        {
                            printf("erro write main.c");
                            exit(1);
                        }
                        printf("enviado ao cliente: %s\n", bufsend);
                    }
                }
                maxclits++;               /*passa para a próxima posição dos vizInt[]*/
                FD_CLR(server_fd, &rfds); // fechar fd do server
            }
        }
    }
    return 0;
}