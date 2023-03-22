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

void help()
{
    printf("Usage: ./cot <IP> <TCP> <regIP> <regUDP>\n");
    printf("Usage: ./cot <IP> <TCP>\n");
    printf("Usage: ./cot\n");
    exit(1);
}
void st()
{
    printf("show topology\n");
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
    int errcode, maxclits = 1, server_fd = -1, k = 0; /*k->counter VizInternos*/ /*server_fd->serverFD*/
    struct sockaddr addr;
    socklen_t addrlen;
    char buffer[500]; // ver melhor o tamanho do buffer
    fd_set rfds;      // create file descriptors set
    // create TCP server
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        printf("erro socket main.c");
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    printf("socket serverfd:%d\n", server_fd);
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;       // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP socket
    hints.ai_flags = AI_PASSIVE;
    if ((errcode = getaddrinfo(NULL, TCP, &hints, &res)) != 0)
    {
        printf("erro get addrinfo main.c");
        exit(1);
    }                                                         /*error*/
    if (bind(server_fd, res->ai_addr, res->ai_addrlen) == -1) // conecta o socket ao endereço
    {
        printf("erro bind main.c");
        exit(1);
    }                               /*error*/
    if (listen(server_fd, 5) == -1) // 5 é o numero de conexões que podem estar em espera
    {
        printf("erro get addrinfo main.c");
        exit(1);
    } /*error*/
    char strV[20], net[20], id[20];
    for (int i = 0; i < 98; i++)
    {
        node.vizInt[i].fd = -1;
    }
    while (1)
    {
        FD_ZERO(&rfds);
        // add stdin, server socket to set
        FD_SET(STDIN_FILENO, &rfds);
        FD_SET(server_fd, &rfds);
        // wait for activity on one of the file descriptors
        int max_fd = (STDIN_FILENO > server_fd) ? STDIN_FILENO : server_fd, Nsel = 1;
        for (int i = 0; i < maxclits; i++)
        {
            if (node.vizInt[i].fd > 0)
            {
                printf("fdset:%d", i);
                FD_SET(node.vizInt[i].fd, &rfds);
                if (node.vizInt[i].fd > max_fd)
                {
                    max_fd = node.vizInt[i].fd;
                }
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
                fgets(buffer, sizeof(buffer), stdin);
                printf("User input: %s", buffer);
                sscanf(buffer, "%s", strV);
                if (strcmp(strV, "join") == 0) // join net id
                {
                    sscanf(buffer, "%s %s %s", strV, net, id);
                    reg(net, id, IP, TCP);
                }
                else if (strcmp(strV, "djoin") == 0) // djoin net id bootid bootIP bootTCP
                {
                    char bootid[3], bootIP[16], bootTCP[5];
                    sscanf(buffer, "%s %s %s %s %s %s", strV, net, id, bootid, bootIP, bootTCP);
                    djoin(net, id, IP, TCP, bootid, bootIP, bootTCP);
                }
                else if (strcmp(strV, "create") == 0) // create name
                {
                    char name[101];
                    sscanf(buffer, "%s %s", strV, name);
                    create(name, flagName);
                    flagName++;
                }
                else if (strcmp(strV, "delete") == 0) // delete name
                {
                    char name[101];
                    sscanf(buffer, "%s %s", strV, name);
                    delete (name, flagName);
                    flagName--;
                }
                else if (strcmp(strV, "get") == 0) // get dest name
                {
                    char dest[4], name[101];
                    sscanf(buffer, "%s %s %s", strV, dest, name); // falta mandar a mensagem para os outros nós
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
                    sscanf(buffer, "%s %s %s", strV, net, id);
                    leave(net, id, IP, TCP, maxclits);
                }
                else if (strcmp(strV, "show") == 0) // exit
                {
                    sscanf(buffer, "%s %s %s", strV, net, id);
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
            }
            for (int i = 0; i < 98; i++)
            {
                if (FD_ISSET(node.vizInt[i].fd, &rfds))
                {
                    char conv[100];
                    int n = -1;
                    printf("INTERN ISSET ID:%s FD:%d\n", node.vizInt[i].IDv, node.vizInt[i].fd);
                    n = read(node.vizExt.fd, conv, 100);
                    if (n == -1)
                    {
                        printf("Error read conversa");
                        exit(1);
                    }
                    printf("O que recebe do que saiu:%s\n", conv);
                    if (strcmp(conv, "0") == 0)
                    {
                        printf("Avisou que saiu\n");
                    }
                }
                FD_CLR(node.vizInt[i].fd, &rfds);
            }
            // check if server socket is ready for accepting new connections
            if (FD_ISSET(server_fd, &rfds))
            {
                printf("SERVER isSET\n");
                ssize_t g;
                addrlen = sizeof(addr);
                int n = 0;
                char Wbuffer[100], cmd[10], bufsend[100] = "";
                if (node.flagVaz == 1)
                {
                    printf("Entra no VAZIO\n");
                    node.vizExt.fd = accept(server_fd, &addr, &addrlen);
                    if ((node.vizExt.fd == -1))
                    { /*error*/
                        printf("EXT erro accept main.c");
                        exit(1);
                    }
                    n = read(node.vizExt.fd, Wbuffer, 100); //  vai ser 0 quando o cliente fechar a ligação
                    printf("VAZIO: %s\n", Wbuffer);
                }
                else
                {
                    printf("Entra no NAO VAZIO\n");
                    node.vizInt[k].fd = accept(server_fd, &addr, &addrlen);
                    if (node.vizInt[k].fd == -1)
                    { /*error*/
                        printf("INT erro accept main.c");
                        exit(1);
                    }
                    n = read(node.vizInt[k].fd, Wbuffer, 100); //  vai ser 0 quando o cliente fechar a ligação
                    printf("NOT VAZIO: %s\n", Wbuffer);
                }
                if (n == -1)
                {
                    printf("erro read main.c");
                    exit(1);
                }
                sscanf(Wbuffer, "%s", cmd);
                if (strcmp(cmd, "NEW") == 0) // se alguém se ligar ao server
                {
                    sscanf(Wbuffer, "%s %s %s %s", cmd, node.vizInt[k].IDv, node.vizInt[k].IPv, node.vizInt[k].Portv);
                    printf("IDv: %s, IPv: %s Portv: %s\n", node.vizInt[k].IDv, node.vizInt[k].IPv, node.vizInt[k].Portv);
                    if (node.flagVaz == 1) // Apenas 2 nós na rede
                    {
                        strcpy(node.vizBackup.IDv, id);
                        strcpy(node.vizBackup.IPv, IP);
                        strcpy(node.vizBackup.Portv, TCP);
                        sscanf(Wbuffer, "%s %s %s %s", cmd, node.vizExt.IDv, node.vizExt.IPv, node.vizExt.Portv);
                        g = sprintf(bufsend, "EXTERN %s %s %s\n", node.vizExt.IDv, node.vizExt.IPv, node.vizExt.Portv);
                        n = write(node.vizExt.fd, bufsend, g);
                        if (n == -1)
                        {
                            printf("erro write main.c");
                            exit(1);
                        }
                        k++; /*passa para a próxima posição dos vizInt[]*/
                        printf("enviado ao cliente: %s\n", bufsend);
                    }
                    else // + de 2 nós na rede
                    {
                        g = sprintf(bufsend, "EXTERN %s %s %s\n", node.vizExt.IDv, node.vizExt.IPv, node.vizExt.Portv);
                        n = write(node.vizInt[k].fd, bufsend, g);
                        if (n == -1)
                        {
                            printf("erro write main.c");
                            exit(1);
                        }
                        k++; /*passa para a próxima posição dos vizInt[]*/
                        printf("enviado ao cliente: %s\n", bufsend);
                    }
                }
                else if (strcmp(cmd, "EXTERN") == 0) /*Acho que o servidor numca entra aqui ?apagar?*/
                {
                    printf("Entrou no EXTERN server");
                    sscanf(Wbuffer, "%s %s %s %s", cmd, node.vizBackup.IDv, node.vizBackup.IPv, node.vizBackup.Portv);
                }
                maxclits++;
            }
            FD_CLR(server_fd, &rfds);
            server_fd = -1;              // fechar fd do server
            FD_CLR(STDIN_FILENO, &rfds); // fechar fd do stdin
        }
    }
    return 0;
}