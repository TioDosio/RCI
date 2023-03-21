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
    int errcode, maxclits = 0, server_fd, client_fds[98], j = 0, k = 0; /*j->count array client_fds[]*/ /*k->counter VizInternos*/ /*server_fd->serverFD*/
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
        client_fds[i] = -1;
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
            if (client_fds[i] > 0)
            {
                FD_SET(client_fds[i], &rfds);
                if (client_fds[i] > max_fd)
                {
                    max_fd = client_fds[i];
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
                fgets(buffer, sizeof(buffer), stdin);
                printf("User input: %s", buffer);
                sscanf(buffer, "%s", strV);
                if (strcmp(strV, "join") == 0) // join net id
                {
                    sscanf(buffer, "%s %s %s", strV, net, id);
                    reg(net, id, IP, TCP); // inet
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
                    st(); // nao feito
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
                    leave(net, id, IP, TCP, client_fds);
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
                else
                {
                    printf("Invalid command\n");
                }
            }
            for (int i = 0; i < maxclits; i++)
            {
                if (FD_ISSET(client_fds[i], &rfds))
                {
                    printf("entrou no client_fds: %d\n", client_fds[i]);
                }
                FD_CLR(client_fds[i], &rfds);
            }
            // check if server socket is ready for accepting new connections
            if (FD_ISSET(server_fd, &rfds))
            {
                ssize_t g;
                addrlen = sizeof(addr);
                client_fds[j] = accept(server_fd, &addr, &addrlen);
                if (client_fds[j] == -1)
                { /*error*/
                    printf("erro accept main.c");
                    exit(1);
                }
                int n = 0;
                char Wbuffer[100], cmd[10], bufsend[100] = "";
                n = read(client_fds[j], Wbuffer, 100); //  vai ser 0 quando o cliente fechar a ligação
                if (n == -1)
                {
                    printf("erro read main.c");
                    exit(1);
                }
                printf("Chegou antes do IF Wbuffer:%s\n", Wbuffer);
                sscanf(Wbuffer, "%s", cmd);
                if (strcmp(cmd, "NEW") == 0)
                {

                    if (node.flagVaz == 1)
                    {
                        sscanf(Wbuffer, "%s %s %s %s", cmd, node.vizExt.IDv, node.vizExt.IPv, node.vizExt.Portv);
                        printf("entrei %s %s %s\n", node.vizExt.IDv, node.vizExt.IPv, node.vizExt.Portv);
                        g = sprintf(bufsend, "EXTERN %s %s %s", node.vizExt.IDv, node.vizExt.IPv, node.vizExt.Portv);
                    }
                    else
                    {
                        g = sprintf(bufsend, "EXTERN %s %s %s", node.vizExt.IDv, node.vizExt.IPv, node.vizExt.Portv);
                    }
                    n = write(client_fds[j], bufsend, g);
                    printf("bufsend: %s", bufsend);
                    if (n == -1)
                    {
                        printf("erro write main.c");
                        exit(1);
                    }
                    k++; /*passa para a próxima posição dos vizInt[]*/
                    printf("interno: %s %s %s\n", node.vizInt[k].IDv, node.vizInt[k].IPv, node.vizInt[k].Portv);
                    printf("enviado: %s", bufsend);
                }
                else if (strcmp(cmd, "EXTERN") == 0)
                {
                    sscanf(cmd, "%s %s %s %s", cmd, node.vizBackup.IDv, node.vizBackup.IPv, node.vizBackup.Portv);
                }
                maxclits++;
                j++;
            }
            FD_CLR(STDIN_FILENO, &rfds);
            FD_CLR(server_fd, &rfds);
        }
    }
    return 0;
}