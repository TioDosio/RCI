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
int maxInter = 0; // Mudar

void help()
{
    printf("Usage: ./cot <IP> <TCP> <regIP> <regUDP>\n");
    printf("Usage: ./cot <IP> <TCP>\n");
    printf("Usage: ./cot\n");
    exit(1);
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
    int errcode, server_fd = -2, n; /*maxInter->counter VizInternos*/ /*server_fd->serverFD*/
    struct sockaddr addr;
    socklen_t addrlen;
    char bufstdin[100]; // ver melhor o tamanho do buffer
    fd_set rfds;        // create file descriptors set
    // create TCP server
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) // cria o socket de servidor para receber as conexões
    {
        printf("erro socket main.c");
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;       // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP socket
    hints.ai_flags = AI_PASSIVE;
    if ((errcode = getaddrinfo(NULL, TCP, &hints, &res)) != 0) // preenche a estrutura res com o endereço do socket
    {
        printf("erro get addrinfo main.c");
        exit(1);
    }
    if (bind(server_fd, res->ai_addr, res->ai_addrlen) == -1) // associa o socket ao endereço
    {
        printf("erro bind main.c");
        exit(1);
    }
    if (listen(server_fd, 5) == -1) // 5 é o numero de conexões que podem estar em espera
    {
        printf("erro get addrinfo main.c");
        exit(1);
    }
    char strV[20], net[20], id[3];
    for (int i = 0; i < 98; i++) // inicializar vizinhos internos
    {
        node.vizInt[i].fd = -2;
        FD_SET(node.vizInt[i].fd, &rfds);
    }
    node.vizExt.fd = -2; // inicializar vizinho externo
    strcpy(node.vizExt.IDv, "");
    strcpy(node.vizExt.IPv, "");
    strcpy(node.vizExt.Portv, "");
    while (1)
    {
        // FD_ZERO(&rfds); // para que serve?
        FD_SET(STDIN_FILENO, &rfds);                                        // add stdin to set
        FD_SET(server_fd, &rfds);                                           // add server_fd to set
        int max_fd = (STDIN_FILENO > server_fd) ? STDIN_FILENO : server_fd; // max_fd é o maior fd
        int Nsel = 1;                                                       // Nsel é o numero de file descriptors prontos e que
        for (int i = 0; i < 98; i++)
        {
            if (node.vizInt[i].fd > 0)
            {
                printf("INTfdset:%d", node.vizInt[i].fd);
                FD_SET(node.vizInt[i].fd, &rfds);
                max_fd = node.vizInt[i].fd;
            }
        }
        if ((Nsel = select(max_fd + 1, &rfds, NULL, NULL, NULL)) < 0) // está atento aos file descriptors
        {
            printf("erro select main.c");
            perror("error select");
            exit(EXIT_FAILURE);
        }
        for (int ns = 0; ns < Nsel; ns++) // Nsel é o numero de file descriptors prontos
        {
            if (FD_ISSET(STDIN_FILENO, &rfds)) //  check if stdin is ready for reading
            {
                printf("stdin isSET\n");
                fgets(bufstdin, 100, stdin);
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
                    char name[101] = "";
                    sscanf(bufstdin, "%s %s", strV, name);
                    create(name, flagName);
                    flagName++;
                }
                else if (strcmp(strV, "delete") == 0) // delete name
                {
                    char name[101] = "";
                    sscanf(bufstdin, "%s %s", strV, name);
                    delete (name, flagName);
                    flagName--;
                }
                else if (strcmp(strV, "get") == 0) // get dest name
                {
                    char dest[4], name[101] = "";
                    sscanf(bufstdin, "%s %s %s", strV, dest, name); // falta mandar a mensagem para os outros nós
                    printf("dest:%s name:%s id:%s", dest, name, id);
                    get(dest, id, name);
                }
                else if ((strcmp(strV, "show topology") == 0) || (strcmp(strV, "st") == 0)) // show topology (st)
                {
                    showTopo(maxInter);
                }
                else if ((strcmp(strV, "show names") == 0) || (strcmp(strV, "sn") == 0)) // show names (sn)
                {
                    showNames(flagName);
                }
                else if ((strcmp(strV, "show routing") == 0) || (strcmp(strV, "sr") == 0)) // show routing (sr)
                {
                    printf("show routing não implementado\n");
                }
                else if (strcmp(strV, "leave") == 0) // leave net id
                {
                    sscanf(bufstdin, "%s %s %s", strV, net, id);
                    leave(net, id, IP, TCP, maxInter);
                }
                else if (strcmp(strV, "show") == 0) // exit
                {
                    sscanf(bufstdin, "%s %s %s", strV, net, id);
                    show(0, net, id, IP, TCP);
                }
                else if (strcmp(strV, "exit") == 0) // exit
                {
                    exit(0);
                }
                else
                {
                    printf("Invalid command\n");
                }
                FD_CLR(STDIN_FILENO, &rfds); // fechar fd do stdin
            }
            for (int i = 0; i < 99; i++) // check if vizinhos internos are ready for reading
            {
                if (FD_ISSET(node.vizInt[i].fd, &rfds)) // maybe meter o que ta ca dentro numa fs :)
                {
                    char conv[100];
                    int n = 0;
                    printf("INT BAN FD:%d", node.vizInt[i].fd);
                    n = read(node.vizInt[i].fd, conv, 100); // ler o que o vizinho interno enviou
                    if (n == -1)
                    {
                        printf("Error read conversa");
                        exit(1);
                    }
                    printf("O que recebe do que saiu:%s\n", conv);
                    if (strcmp(conv, "0") == 0) // se recebeu 0, avisou que saiu
                    {
                        printf("Avisou que saiu (0)\n");
                    }
                    strcpy(node.vizInt[i].IDv, "");
                    strcpy(node.vizInt[i].IPv, "");
                    strcpy(node.vizInt[i].Portv, "");
                    for (int j = i; j <= maxInter; j++) // shift left
                    {
                        strcpy(node.vizInt[j].IDv, node.vizInt[j + 1].IDv);
                        strcpy(node.vizInt[j].IPv, node.vizInt[j + 1].IPv);
                        strcpy(node.vizInt[j].Portv, node.vizInt[j + 1].Portv);
                    }
                    maxInter--;
                    FD_CLR(node.vizInt[i].fd, &rfds); // fechar fd do vizinho interno
                    node.vizInt[i].fd = -2;           // marcar como fechado
                }
            }
            if (FD_ISSET(node.vizExt.fd, &rfds)) // maybe meter o que ta ca dentro numa fs :)
            {
                printf("FD EXT ISSET\n");
                char conv[100], buf[100];
                int n = 0;
                printf("EXT AN FD:%d", node.vizExt.fd);
                n = read(node.vizExt.fd, conv, 100); // ler o que o vizinho externo enviou
                if (n == -1)
                {
                    printf("Error read conversa");
                    exit(1);
                }
                printf("O que recebe do que saiu:%s\n", conv);
                if (strcmp(conv, "0") == 0) // se recebeu 0, avisou que saiu
                {
                    printf("Avisou que saiu (0)\n");
                }
                if (strcmp(node.vizExt.IDv, "") != 0) // não ancora
                {
                    strcpy(node.vizExt.IDv, "");
                    strcpy(node.vizExt.IPv, "");
                    strcpy(node.vizExt.Portv, "");
                    strcpy(node.vizExt.IDv, node.vizBackup.IDv);
                    strcpy(node.vizExt.IPv, node.vizBackup.IPv);
                    strcpy(node.vizExt.Portv, node.vizBackup.Portv);
                    client_tcp(id, net, TCP);
                    sprintf(buf, "EXTERN %s %s %s", node.vizBackup.IDv, node.vizBackup.IPv, node.vizBackup.Portv);
                    for (int i = 0; i < maxInter; i++)
                    {
                        write(node.vizInt[i].fd, buf, 100);
                    }
                }
                if ((maxInter == 0) && (strcmp(id, node.vizBackup.IDv) == 0))
                {
                    // ancora
                }

                FD_CLR(node.vizExt.fd, &rfds); // fechar fd do vizinho externo
                node.vizExt.fd = -2;           // marcar como fechado
            }
            if (FD_ISSET(server_fd, &rfds)) // check if server is ready for reading
            {
                printf("SERVER IS_SET\n");
                ssize_t g;
                addrlen = sizeof(addr);
                char bufRead[100], cmd[10], bufsend[100] = "";
                if (node.flagVaz == 1) // se for o primeiro nó a entrar
                {
                    printf("Entra no VAZIO\n");
                    node.vizExt.fd = accept(server_fd, &addr, &addrlen); // aceitar a ligação
                    if ((node.vizExt.fd == -1))
                    {
                        printf("EXT erro accept main.c");
                        exit(1);
                    }
                    n = read(node.vizExt.fd, bufRead, 100); // ler o que a nova ligação enviou
                    printf("VAZIO: %s\n", bufRead);
                    sscanf(bufRead, "%s %s %s %s", cmd, node.vizExt.IDv, node.vizExt.IPv, node.vizExt.Portv);
                }
                else // se já houver nós na rede
                {
                    printf("Entra no NAO VAZIO\n");
                    node.vizInt[maxInter].fd = accept(server_fd, &addr, &addrlen); // aceitar a ligação
                    if (node.vizInt[maxInter].fd == -1)
                    {
                        printf("INT erro accept main.c");
                        exit(1);
                    }
                    n = read(node.vizInt[maxInter].fd, bufRead, 100); // ler o que a nova ligação enviou
                    printf("NOT VAZIO: %s\n", bufRead);
                    sscanf(bufRead, "%s %s %s %s", cmd, node.vizInt[maxInter].IDv, node.vizInt[maxInter].IPv, node.vizInt[maxInter].Portv);
                }
                if (n == -1)
                {
                    printf("erro read main.c");
                    exit(1);
                }
                if (node.flagVaz == 1) // Apenas 2 nós na rede
                {
                    g = sprintf(bufsend, "EXTERN %s %s %s\n", node.vizExt.IDv, node.vizExt.IPv, node.vizExt.Portv);
                    n = write(node.vizExt.fd, bufsend, g); // enviar o nosso externo ao vizinho externo
                    if (n == -1)
                    {
                        printf("erro write main.c");
                        exit(1);
                    }
                    printf("enviado ao cliente: %s\n", bufsend);
                }
                else // + de 2 nós na rede
                {
                    g = sprintf(bufsend, "EXTERN %s %s %s\n", node.vizExt.IDv, node.vizExt.IPv, node.vizExt.Portv);
                    n = write(node.vizInt[maxInter].fd, bufsend, g); // enviar o nosso externo ao vizinho interno
                    if (n == -1)
                    {
                        printf("erro write main.c");
                        exit(1);
                    }
                    maxInter++; /*passa para a próxima posição dos vizInt[]*/
                }
                FD_CLR(server_fd, &rfds); // fechar fd do server
                server_fd = -2;
            }
        }
    }
    return 0;
}