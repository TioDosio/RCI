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
int main(int argc, char *argv[])
{
    char IP[20];     // IP do TCP que é dado
    char TCP[20];    // Porto do TCP que é dado
    char regIP[20];  // IP do UDP pode ou não ser dado
    char regUDP[20]; // Porto do UDP que pode ou não ser dado
    switch (argc)    // leitura dos argumentos de entrada
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
    int errcode, server_fd = -2; /*node.maxInter->counter VizInternos*/ /*server_fd->serverFD*/
    struct sockaddr addr;
    socklen_t addrlen;
    char bufstdin[100]; // ver melhor o tamanho do buffer
    fd_set rfds, fds;   // create file descriptors set
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
    char strV[20], net[20], id[3];
    for (int i = 0; i < 98; i++) // inicializar o array de vizinhos internos a -2
    {
        node.vizInt[i].fd = -2;
    }
    node.vizExt.fd = -2;          // inicializar o vizinho externo a -2
    for (int i = 0; i < 100; i++) // inicializar o array de tabExp a -1
    {
        node.tabExp[i] = -1;
    }
    strcpy(node.vizExt.IDv, "");
    strcpy(node.vizExt.IPv, "");
    strcpy(node.vizExt.Portv, "");
    while (1)
    {
        FD_ZERO(&rfds);
        FD_SET(STDIN_FILENO, &rfds);
        FD_SET(server_fd, &rfds);
        if (node.vizExt.fd > 0)
        {
            FD_SET(node.vizExt.fd, &rfds);
        }
        // int max_fd = (STDIN_FILENO > server_fd) ? STDIN_FILENO : server_fd;
        int Nsel = 1;
        for (int i = 0; i < 98; i++)
        {
            if (node.vizInt[i].fd > 0)
            {
                printf("INTfdset:%d\n", node.vizInt[i].fd);
                FD_SET(node.vizInt[i].fd, &rfds);
                // max_fd = node.vizInt[i].fd;
            }
        }
        fds = rfds;
        if ((Nsel = select(99 + 1, &fds, NULL, NULL, NULL)) < 0)
        {
            printf("erro select main.c");
            perror("error select");
            exit(EXIT_FAILURE);
        }
        for (int counter = 0; counter < Nsel; counter++)
        {
            // check if stdin is ready for reading
            if (FD_ISSET(STDIN_FILENO, &fds))
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
                    create(name);
                    node.flagName++;
                }
                else if (strcmp(strV, "delete") == 0) // delete name
                {
                    char name[101] = "";
                    sscanf(bufstdin, "%s %s", strV, name);
                    delete (name);
                    node.flagName--;
                }
                else if (strcmp(strV, "get") == 0) // get dest name
                {
                    char dest[4], name[101] = "";
                    sscanf(bufstdin, "%s %s %s", strV, dest, name); // falta mandar a mensagem para os outros nós
                    printf("dest:%s name:%s orig:%s", dest, name, id);
                    get(dest, id, name);
                }
                else if ((strcmp(strV, "show topology") == 0) || (strcmp(strV, "st") == 0)) // show topology (st)
                {
                    showTopo(node.maxInter);
                }
                else if ((strcmp(strV, "show names") == 0) || (strcmp(strV, "sn") == 0)) // show names (sn)
                {
                    showNames();
                }
                else if ((strcmp(strV, "show routing") == 0) || (strcmp(strV, "sr") == 0)) // show routing (sr)
                {
                    showRouting();
                }
                else if (strcmp(strV, "leave") == 0) // leave net id
                {
                    sscanf(bufstdin, "%s %s %s", strV, net, id);
                    leave(net, id, IP, TCP);
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
            }
            for (int i = 0; i < 98; i++)
            {
                if (FD_ISSET(node.vizInt[i].fd, &fds)) // check if vizInt is ready for reading
                {
                    int n = 0;
                    char bufR[100], cmd[20], bufW[100];
                    printf("INTERNO[%d]:%d\n", i, node.vizInt[i].fd);
                    node.vizInt[i].ctrbufsize += read(node.vizInt[i].fd, node.vizInt[i].ctrbuf, 100); // NEW net id\n NEW net id\n
                    strcat(bufR, node.vizInt[i].ctrbuf);
                    printf("bufR:%s\n", bufR);
                    if (bufR[node.vizInt[i].ctrbufsize - 1] == '\n')
                    {
                        sscanf(bufR, "%s", cmd);
                        if (strcmp(cmd, "NEW") == 0) /*Como só há 2 nós na rede são ancoras então o NEW é guardado com Externo*/
                        {
                            sscanf(bufR, "%s %s %s %s", cmd, node.vizInt[i].IDv, node.vizInt[i].IPv, node.vizInt[i].Portv); // recebe NEW e guarda como interno
                            sprintf(bufW, "EXTERN %s %s %s\n", node.vizExt.IDv, node.vizExt.IPv, node.vizExt.Portv);        // mete seu EXT no buffer
                            write(node.vizInt[i].fd, bufW, strlen(bufW));                                                   // Envia EXTERN para o Intern para ser o Back dele
                            node.tabExp[atoi(node.vizInt[i].IDv)] = atoi(node.vizInt[i].IDv);                               // Adiciona o novo vizinho na tabela de exp
                        }
                        else if (strcmp(cmd, "QUERY") == 0) // QUERY DEST ORIG NAME
                        {
                            char destR[3], origR[3], nameR[100];
                            sscanf(bufR, "%s %s %s %s", cmd, destR, origR, nameR);
                            printf("QUERY RECEBIDO, %s %s %s\n", destR, origR, nameR);
                        }
                        else if (n == 0)
                        {
                            printf("vizInt[%d] disconnected\n", i);
                            node.vizInt[i].fd = -2;
                            strcpy(node.vizInt[i].IDv, "");
                            strcpy(node.vizInt[i].IPv, "");
                            strcpy(node.vizInt[i].Portv, "");
                            strcpy(node.vizInt[i].ctrbuf, "");
                            for (int j = i; j < node.maxInter; j++)
                            {
                                node.vizInt[j] = node.vizInt[j + 1];
                            }
                            node.maxInter--;
                        }
                        else if (strcmp(cmd, "EXTERN") == 0)
                        {
                            printf("EXTERN WTF\n");
                        }
                        strcpy(bufR, "");
                        node.vizInt[i].ctrbufsize = 0;
                    }
                }
            }
            if (FD_ISSET(node.vizExt.fd, &fds)) // check if vizExt is ready for reading
            {
                int n = 0;
                printf("EXTERNO:%d\n", node.vizExt.fd);
                char bufR[100], cmd[20], bufW[100];
                strcpy(bufR, "");
                strcpy(bufW, "");
                strcpy(cmd, "");
                node.vizExt.ctrbufsize += read(node.vizExt.fd, node.vizExt.ctrbuf, 100);
                strcat(bufR, node.vizExt.ctrbuf);
                printf("bufR:%s\n", bufR);
                if (bufR[node.vizExt.ctrbufsize - 1] == '\n') // a mensagem foi toda recebida
                {
                    printf("ENTRA NO ID DO BARRAN EXTERNO\n");
                    sscanf(bufR, "%s", cmd);
                    if (strcmp(cmd, "NEW") == 0) /*Como só há 2 nós na rede são ancoras então o NEW é guardado com Externo*/
                    {
                        sscanf(bufR, "%s %s %s %s", cmd, node.vizExt.IDv, node.vizExt.IPv, node.vizExt.Portv);
                        sprintf(bufW, "EXTERN %s %s %s\n", node.vizExt.IDv, node.vizExt.IPv, node.vizExt.Portv);
                        write(node.vizExt.fd, bufW, strlen(bufW));
                        node.tabExp[atoi(node.vizExt.IDv)] = atoi(node.vizExt.IDv);
                    }
                    else if (strcmp(cmd, "EXTERN") == 0)
                    {
                        sscanf(bufR, "%s %s %s %s", cmd, node.vizBackup.IDv, node.vizBackup.IPv, node.vizBackup.Portv);
                    }
                    else if (strcmp(cmd, "QUERY") == 0) // QUERY DEST ORIG NAME
                    {
                        char destR[3], origR[3], nameR[100];
                        sscanf(bufR, "%s %s %s %s", cmd, destR, origR, nameR);
                        printf("QUERY RECEBIDO, %s %s %s\n", destR, origR, nameR);
                    }
                    else if (n == 0)
                    {
                        printf("vizExt disconnected\n");
                    }
                    strcpy(bufR, "");
                    node.vizExt.ctrbufsize = 0;
                }
            }
            if (FD_ISSET(server_fd, &fds))
            {
                printf("SERVER IS_SET\n");
                addrlen = sizeof(addr);
                if (node.flagVaz == 1) // Sou o único da rede e ligam-se a mim
                {
                    printf("Sou o único da rede e ligam-se a mim\n");
                    node.vizExt.fd = accept(server_fd, &addr, &addrlen);
                    if ((node.vizExt.fd == -1))
                    {
                        printf("EXT erro accept main.c");
                        exit(1);
                    }
                }
                else // mais de 2 nós
                {
                    printf("Aceita um nó (a rede já nao sou só eu)\n");
                    node.vizInt[node.maxInter].fd = accept(server_fd, &addr, &addrlen);
                    if (node.vizInt[node.maxInter].fd == -1)
                    {
                        printf("INT erro accept main.c");
                        exit(1);
                    }
                    node.maxInter++;
                }
            }
        }
    }
    return 0;
}