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
    char strV[20], net[20];
    for (int i = 0; i < 98; i++) // inicializar o array de vizinhos internos a -2
    {
        node.vizInt[i].fd = -2;
    }
    node.vizExt.fd = -2;          // inicializar o vizinho externo a -2
    for (int i = 0; i < 100; i++) // inicializar o array de tabExp a -1
    {
        node.tabExp[i] = -2;
    }
    strcpy(node.vizExt.IDv, "");
    strcpy(node.vizExt.IPv, "");
    strcpy(node.vizExt.Portv, "");
    ;
    while (1)
    {
        FD_ZERO(&rfds);
        FD_SET(STDIN_FILENO, &rfds);
        if (server_fd > 0)
        {
            FD_SET(server_fd, &rfds);
        }
        int max_fd = (STDIN_FILENO > server_fd) ? STDIN_FILENO : server_fd; // coloca o maior fd no max_fd entre i sdtin e o server
        if (node.vizExt.fd > 0)
        {
            FD_SET(node.vizExt.fd, &rfds); // coloca o vizinho externo no rfds
            max_fd = node.vizExt.fd;       // se houver vizinho externo coloca o fd como o max_fd
        }
        int Nsel = 0;
        for (int i = 0; i < 98; i++) // coloca os vizinhos internos que estão ligados no rfds
        {
            if (node.vizInt[i].fd > 0)
            {
                FD_SET(node.vizInt[i].fd, &rfds);
                if (node.vizInt[i].fd > max_fd)
                {
                    max_fd = node.vizInt[i].fd; // se houver vizinhos internos coloca o fd como o max_fd
                }
            }
        }
        fds = rfds;
        if ((Nsel = select(max_fd + 1, &fds, NULL, NULL, NULL)) < 0) // select para ver se há algo para ler
        {
            printf("erro select main.c");
            perror("error select");
            exit(EXIT_FAILURE);
        }
        for (int counter = 0; counter < Nsel; counter++) // se houver mais que uma coisa para ler ao mesmo tempo o select retorna o Nsel e repete aqui as vazes necessarias
        {
            if (FD_ISSET(STDIN_FILENO, &fds)) // check if stdin is ready for reading
            {
                strcpy(bufstdin, "");
                fgets(bufstdin, 100, stdin);
                sscanf(bufstdin, "%s", strV);
                if (strcmp(strV, "join") == 0) // join net id
                {
                    sscanf(bufstdin, "%s %s %s", strV, net, node.id);
                    reg(net, IP, TCP);
                }
                else if (strcmp(strV, "djoin") == 0) // djoin net id bootid bootIP bootTCP
                {
                    char bootid[3], bootIP[16], bootTCP[5], dnet[4];
                    sscanf(bufstdin, "%s %s %s %s %s %s", strV, dnet, node.id, bootid, bootIP, bootTCP);
                    djoin(dnet, IP, TCP, bootid, bootIP, bootTCP);
                }
                else if (strcmp(strV, "create") == 0) // create name
                {
                    char name[101] = "";
                    sscanf(bufstdin, "%s %s", strV, name);
                    create(name);
                }
                else if (strcmp(strV, "delete") == 0) // delete name
                {
                    char name[101] = "";
                    sscanf(bufstdin, "%s %s", strV, name);
                    delete (name);
                }
                else if (strcmp(strV, "get") == 0) // get dest name
                {
                    char dest[4], name[101] = "";
                    sscanf(bufstdin, "%s %s %s", strV, dest, name); // falta mandar a mensagem para os outros nós
                    printf("dest:%s name:%s orig:%s", dest, name, node.id);
                    get(dest, name);
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
                    sscanf(bufstdin, "%s %s %s", strV, net, node.id);
                    leave(net, IP, TCP);
                }
                else if (strcmp(strV, "show") == 0) // exit
                {
                    sscanf(bufstdin, "%s %s %s", strV, net, node.id);
                    show(0, net, IP, TCP);
                }
                else if (strcmp(strV, "exit") == 0) // exit
                {
                    exit(0);
                }
                else if (strcmp(strV, "ban") == 0)
                {
                    if (node.vizExt.fd >= 0)
                    {
                        printf("FDext:%d\n", node.vizExt.fd);
                    }

                    for (int i = 0; i < 98; i++)
                    {
                        if (node.vizInt[i].fd >= 0)
                        {
                            printf("FD[%d]%d\n", i, node.vizInt[i].fd);
                        }
                    }
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
                    int fdR = -2;
                    char bufR[100], cmd[20], bufW[100];
                    strcpy(bufR, "");
                    node.vizInt[i].ctrbufsize += read(node.vizInt[i].fd, node.vizInt[i].ctrbuf, 100); // NEW net id\n NEW net id\n
                    strcat(bufR, node.vizInt[i].ctrbuf);
                    printf("INTERNO[%d]:%d->R:%s\n", i, node.vizInt[i].fd, bufR);
                    if (bufR[node.vizInt[i].ctrbufsize - 1] == '\n')
                    {
                        sscanf(bufR, "%s", cmd);
                        if (strcmp(cmd, "NEW") == 0) /*Como só há 2 nós na rede são ancoras então o NEW é guardado com Externo*/
                        {
                            sscanf(bufR, "%s %s %s %s", cmd, node.vizInt[i].IDv, node.vizInt[i].IPv, node.vizInt[i].Portv); // recebe NEW e guarda como interno
                            sprintf(bufW, "EXTERN %s %s %s\n", node.vizExt.IDv, node.vizExt.IPv, node.vizExt.Portv);        // mete seu EXT no buffer
                            printf("Enviado:%s", bufW);                                                                     ///////////////////////////////////////////////////////////////////////77
                            write(node.vizInt[i].fd, bufW, strlen(bufW));                                                   // Envia EXTERN para o Intern para ser o Back dele
                            node.tabExp[atoi(node.vizInt[i].IDv)] = atoi(node.vizInt[i].IDv);                               // Adiciona o novo vizinho na tabela de exp
                        }
                        else if (strcmp(cmd, "QUERY") == 0) // QUERY DEST ORIG NAME
                        {
                            char destQ[3], origQ[3], nameQ[100];
                            sscanf(bufR, "%s %s %s %s", cmd, destQ, origQ, nameQ);
                            fdR = node.vizInt[i].fd;
                            node.tabExp[atoi(origQ)] = atoi(node.vizInt[i].IDv);
                            query(destQ, origQ, nameQ, fdR);
                        }
                        else if (strcmp(cmd, "CONTENT") == 0)
                        {
                            char destC[3], origC[3], nameC[100];
                            sscanf(bufR, "%s %s %s %s", cmd, origC, destC, nameC);
                            fdR = node.vizInt[i].fd;
                            node.tabExp[atoi(destC)] = atoi(node.vizInt[i].IDv);
                            CNContent(0, destC, origC, nameC, fdR);
                        }
                        else if (strcmp(cmd, "NOCONTENT") == 0)
                        {
                            char destC[3], origC[3], nameC[100];
                            sscanf(bufR, "%s %s %s %s", cmd, origC, destC, nameC);
                            fdR = node.vizInt[i].fd;
                            node.tabExp[atoi(destC)] = atoi(node.vizInt[i].IDv);
                            CNContent(1, destC, origC, nameC, fdR);
                        }
                        else if (strcmp(cmd, "WITHDRAW") == 0)
                        {
                            char idW[3], bufsend[100];
                            int fdR = node.vizInt[i].fd;
                            strcpy(bufsend, "");
                            sscanf(bufR, "%s %s", cmd, idW);
                            wdraw(idW, fdR);
                        }
                        node.vizInt[i].ctrbufsize = 0;
                    }
                    else if (node.vizInt[i].ctrbufsize == 0) // saída de um Vizinho Interno
                    {
                        char bufW[15]; // withdraw id
                        printf("vizInt[%d] disconnected\n", i);
                        sprintf(bufW, "WITHDRAW %s\n", node.vizInt[i].IDv);
                        node.tabExp[atoi(node.vizInt[i].IDv)] = -2;
                        for (int i = 0; i < node.maxInter; i++)
                        {
                            if (node.tabExp[i] == atoi(node.vizInt[i].IDv))
                            {
                                node.tabExp[i] = -2;
                            }
                        }
                        node.vizInt[i].fd = -2;
                        write(node.vizExt.fd, bufW, strlen(bufW));
                        for (int j = 0; j < node.maxInter; j++)
                        {
                            if (node.vizInt[j].fd != -2)
                            {
                                write(node.vizInt[j].fd, bufW, strlen(bufW));
                            }
                        }
                        strcpy(node.vizInt[i].IDv, "");
                        strcpy(node.vizInt[i].IPv, "");
                        strcpy(node.vizInt[i].Portv, "");
                        strcpy(node.vizInt[i].ctrbuf, "");
                        node.vizInt[i].ctrbufsize = 0;
                        for (int j = i; j < node.maxInter; j++)
                        {
                            node.vizInt[j] = node.vizInt[j + 1];
                        }
                        node.maxInter--;
                    }
                }
                FD_CLR(node.vizInt[i].fd, &fds);
            }
            if (FD_ISSET(node.vizExt.fd, &fds)) // check if vizExt is ready for reading
            {
                int fdR = -2, flag = 1;
                char bufR[100], bufM1[30], cmd[20], bufW[100], *ptr, *line;
                printf("EXTERNO:%d\n", node.vizExt.fd);
                strcpy(bufR, "");
                strcpy(bufW, "");
                strcpy(cmd, "");
                node.vizExt.ctrbufsize = read(node.vizExt.fd, node.vizExt.ctrbuf, 100);
                strcat(bufR, node.vizExt.ctrbuf);
                if (bufR[node.vizExt.ctrbufsize - 1] == '\n')
                {
                    line = strtok_r(bufR, "\n", &ptr);
                    strcpy(bufM1, line);
                    printf("bufM1:%s\n", bufM1);
                    printf("EXT-bufR:%s\n", bufR);
                    for (int i = 0; i < flag; i++)
                    {
                        sscanf(bufM1, "%s", cmd);
                        if (strcmp(cmd, "NEW") == 0) /*Como só há 2 nós na rede são ancoras então o NEW é guardado com Externo*/
                        {
                            sscanf(bufM1, "%s %s %s %s", cmd, node.vizExt.IDv, node.vizExt.IPv, node.vizExt.Portv);
                            sprintf(bufW, "EXTERN %s %s %s\n", node.vizExt.IDv, node.vizExt.IPv, node.vizExt.Portv);
                            write(node.vizExt.fd, bufW, strlen(bufW));
                            printf("Enviado: %s", bufW);
                            node.tabExp[atoi(node.vizExt.IDv)] = atoi(node.vizExt.IDv);
                        }
                        else if (strcmp(cmd, "EXTERN") == 0)
                        {
                            sscanf(bufM1, "%s %s %s %s", cmd, node.vizBackup.IDv, node.vizBackup.IPv, node.vizBackup.Portv);
                            printf("Recebido: %s\n", bufM1);
                        }
                        else if (strcmp(cmd, "QUERY") == 0) // QUERY DEST ORIG NAME
                        {
                            char destQ[3], origQ[3], nameQ[100];
                            sscanf(bufM1, "%s %s %s %s", cmd, destQ, origQ, nameQ);
                            node.tabExp[atoi(origQ)] = atoi(node.vizExt.IDv);
                            printf("tabExp[%d]:%d", atoi(origQ), node.tabExp[atoi(origQ)]);
                            fdR = node.vizExt.fd;
                            query(destQ, origQ, nameQ, fdR);
                        }
                        else if (strcmp(cmd, "CONTENT") == 0)
                        {
                            char destC[3], origC[3], nameC[100];
                            sscanf(bufM1, "%s %s %s %s", cmd, origC, destC, nameC);
                            fdR = node.vizExt.fd;
                            node.tabExp[atoi(destC)] = atoi(node.vizExt.IDv);
                            CNContent(0, destC, origC, nameC, fdR);
                        }
                        else if (strcmp(cmd, "NOCONTENT") == 0)
                        {
                            char destC[3], origC[3], nameC[100];
                            sscanf(bufM1, "%s %s %s %s", cmd, origC, destC, nameC);
                            fdR = node.vizExt.fd;
                            node.tabExp[atoi(destC)] = atoi(node.vizExt.IDv);
                            CNContent(1, destC, origC, nameC, fdR);
                        }
                        else if (strcmp(cmd, "WITHDRAW") == 0)
                        {
                            char idW[3], bufsend[100];
                            int fdR = node.vizExt.fd;
                            strcpy(bufsend, "");
                            sscanf(bufM1, "%s %s", cmd, idW);
                            wdraw(idW, fdR);
                        }
                    }
                    node.vizExt.ctrbufsize = 0;
                }
                else if (node.vizExt.ctrbufsize == 0) // saída do vizinho externo
                {
                    char bufW[100]; // withdraw id
                    printf("vizExt disconnected\n");
                    sprintf(bufW, "WITHDRAW %s\n", node.vizExt.IDv);
                    node.tabExp[atoi(node.vizExt.IDv)] = -2;
                    for (int i = 0; i < 100; i++) // atualizar tabela de exp
                    {
                        if (node.tabExp[i] == atoi(node.vizExt.IDv))
                        {
                            node.tabExp[i] = -2;
                        }
                    }
                    for (int j = 0; j < node.maxInter; j++)
                    {
                        if (node.vizInt[j].fd != -2)
                        {
                            write(node.vizInt[j].fd, bufW, strlen(bufW));
                        }
                    }
                    if (strcmp(node.id, node.vizBackup.IDv) != 0) // não somos ancora
                    {
                        strcpy(node.vizExt.IDv, node.vizBackup.IDv);
                        strcpy(node.vizExt.IPv, node.vizBackup.IPv);
                        strcpy(node.vizExt.Portv, node.vizBackup.Portv);
                        client_tcp(IP, TCP);
                        printf("N.A. Ligar Back-> ID:%s IP:%s Porto:%s\n", node.vizExt.IDv, node.vizExt.IPv, node.vizExt.Portv);
                        for (int i = 0; i < node.maxInter; i++)
                        {
                            if (node.vizInt[i].fd != -2)
                            {
                                strcpy(bufW, "");
                                sprintf(bufW, "EXTERN %s %s %s\n", node.vizExt.IDv, node.vizExt.IPv, node.vizExt.Portv);
                                write(node.vizInt[i].fd, bufW, strlen(bufW));
                            }
                        }
                    }
                    else if (node.maxInter != 0) // somos ancora e temos vizinhos internos
                    {
                        printf("Somos Ancora com Int's\n");
                        strcpy(node.vizExt.IDv, node.vizInt[0].IDv);
                        strcpy(node.vizExt.IPv, node.vizInt[0].IPv);
                        strcpy(node.vizExt.Portv, node.vizInt[0].Portv);
                        node.vizExt.fd = node.vizInt[0].fd;
                        for (int j = 0; j < node.maxInter; j++)
                        {
                            node.vizInt[j] = node.vizInt[j + 1];
                        }
                        node.maxInter--;
                        sprintf(bufW, "EXTERN %s %s %s\n", node.vizExt.IDv, node.vizExt.IPv, node.vizExt.Portv);
                        write(node.vizExt.fd, bufW, strlen(bufW));
                        for (int i = 0; i < node.maxInter; i++)
                        {
                            write(node.vizInt[i].fd, bufW, strlen(bufW));
                        }
                        printf("Enviado: %s", bufW);
                    }
                    else
                    {
                        printf("Fiquei sozinho na rede :(\n");
                        strcpy(node.vizExt.IDv, "");
                        strcpy(node.vizExt.IPv, "");
                        strcpy(node.vizExt.Portv, "");
                        strcpy(node.vizBackup.IDv, node.id);
                        strcpy(node.vizBackup.IPv, IP);
                        strcpy(node.vizBackup.Portv, TCP);
                        node.flagVaz = 0;
                        node.vizExt.fd = -2;
                    }
                }
                FD_CLR(node.vizExt.fd, &fds);
            }
            if (FD_ISSET(server_fd, &fds))
            {
                printf("SERVER IS_SET\n");
                addrlen = sizeof(addr);
                if (node.flagVaz == 0) // Sou o único da rede e ligam-se a mim
                {
                    printf("Sou o único da rede e ligam-se a mim\n");
                    node.vizExt.fd = accept(server_fd, &addr, &addrlen);
                    printf("Accept %d\n", node.vizExt.fd); ////////////////////////////
                    if ((node.vizExt.fd == -1))
                    {
                        printf("EXT erro accept main.c");
                        exit(1);
                    }
                    node.flagVaz = 1;
                }
                else if (node.flagVaz == 1) // mais de 2 nós
                {
                    int k;
                    printf("Aceita um nó (a rede já nao sou só eu)\n");
                    k = node.vizInt[node.maxInter].fd = accept(server_fd, &addr, &addrlen);
                    if (k == -1)
                    {
                        printf("INT erro accept main.c");
                    }
                    printf("Accept %d\n", node.vizInt[node.maxInter].fd); //////////////////////////////
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