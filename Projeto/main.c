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
// usar o getaddrinfo para o TCP client em que metemos o IP e Port como char e no servidor o IP como null
// ver
struct NO node;
//char TCP[50];

void help()
{
    printf("Usage: ./cot <IP> <TCP> <regIP> <regUDP>\n");
    printf("Usage: ./cot <IP> <TCP>\n");
    printf("Usage: ./cot\n");
    exit(1);
}
void delete()
{
    printf("deleting\n");
}
void get(char *dest, char *name)
{
    printf("getting\n");
}

void djoin(char *net, char *id, char *bootid, char *bootIP, char *bootTCP) // djoin(net, id, bootid, bootIP, bootTCP);
{
    printf("djoining...\n");
}
void st()
{
    printf("show topology\n");
}
void sn()
{
    printf("show names\n");
}
void sr()
{
    printf("show routing\n");
}

int main(int argc, char *argv[])
{
    char IP[20]; // IP do TCP que é dado
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
        strcpy(IP, argv[1]);  //"127.0.0.1"; ou "127.0.1.1";
        strcpy(TCP, argv[2]); //"59001";
        printf("IP: %s\n", IP);
        printf("TCP: %s\n", TCP);

        break;
    case 5: // Recebe o IP e o TCP e o IP e o UDP
        printf("Com 4 argumentos\n");
        strcpy(IP, argv[1]); //"127.0.0.1"; ou "127.0.1.1";
        strcpy(regIP, argv[3]);
        strcpy(regUDP, argv[4]);
        printf("IP: %s\n", IP);
        printf("TCP: %s\n", TCP);
        break;
    default:
        printf("Argumentos invalidos\n");
        printf("argc %d\n", argc);
        help();
    }
    struct addrinfo hints, *res;
    int server_fd, newfd, errcode, maxclits = 0, clitFDs[10];
    ssize_t n, nw;
    struct sockaddr addr;
    socklen_t addrlen;
    char *ptr, buffer[500]; // ver melhor o tamanho do buffer
    // create TCP server
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;       // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP socket
    hints.ai_flags = AI_PASSIVE;
    if ((errcode = getaddrinfo(NULL, TCP, &hints, &res)) != 0)
        exit(1);                                              /*error*/
    if (bind(server_fd, res->ai_addr, res->ai_addrlen) == -1) // conecta o socket ao endereço
        exit(1);                                              /*error*/
    if (listen(server_fd, 5) == -1)                           // 5 é o numero de conexões que podem estar em espera
        exit(1);                                              /*error*/
    // create file descriptors set
    fd_set fds;
    FD_ZERO(&fds);

    // add stdin, server socket to set
    FD_SET(STDIN_FILENO, &fds);
    FD_SET(server_fd, &fds);
    char strV[20], net[20], id[20];
    while (1)
    {
        // wait for activity on one of the file descriptors
        int max_fd = (STDIN_FILENO > server_fd) ? STDIN_FILENO : server_fd;
        for (int i = 0; i < max_clients; i++)
        {
            if (client_fds[i] > 0)
            {
                FD_SET(client_fds[i], &fds);
                if (client_fds[i] > max_fd)
                {
                    max_fd = client_fds[i];
                }
            }
        }

        if (select(max_fd + 1, &fds, NULL, NULL, NULL) < 0)
        {
            perror("select");
            exit(EXIT_FAILURE);
        }

        // check if stdin is ready for reading
        if (FD_ISSET(STDIN_FILENO, &fds))
        {
            fgets(buffer, sizeof(buffer), stdin);
            printf("User input: %s", buffer);
            sscanf(buffer, "%s", strV);
            if (strcmp(strV, "join") == 0) // join net id
            {
                // clitTCP(IP, TCP);
                sscanf(buffer, "%s %s %s", strV, net, id);
                reg(net, id, IP, TCP); // inet
            }
            else if (strcmp(strV, "djoin") == 0) // djoin net id bootid bootIP bootTCP
            {
                char bootid[3], bootIP[16], bootTCP[5];
                sscanf(buffer, "%s %s %s %s %s %s", strV, net, id, bootid, bootIP, bootTCP);
                djoin(net, id, bootid, bootIP, bootTCP);
            }
            else if (strcmp(strV, "create") == 0) //////////////////////////////////////////// create name -> Criar vários nomes
            {
                char name[101];
                sscanf(buffer, "%s %s", strV, name);
                printf("created name: %s\n", name);
            }
            else if (strcmp(strV, "delete") == 0) // delete name
            {
                // char name[idk] =""; -> strcpy(name[idk], "");
            }
            else if (strcmp(strV, "get") == 0) // get dest name
            {
                char dest[4], name[101];
                sscanf(buffer, "%s %s %s", strV, dest, name);
                get(dest, name);
            }
            else if ((strcmp(strV, "show topology") == 0) || (strcmp(strV, "st\n") == 0)) // show topology (st)
            {
                st();
            }
            else if ((strcmp(strV, "show names") == 0) || (strcmp(strV, "server\n") == 0)) // show names (sn)
            {
                // servTCP(&tcpV, TCP);
                //  sn();
            }
            else if ((strcmp(strV, "show routing") == 0) || (strcmp(strV, "cliente\n") == 0)) // show routing (sr)
            {

                sr();
            }
            else if (strcmp(strV, "leave") == 0) // leave
            {
                sscanf(buffer, "%s %s %s", strV, net, id);
                unreg(net, id, IP, TCP);
            }
            else if (strcmp(strV, "show") == 0) // exit
            {
                sscanf(buffer, "%s %s %s", strV, net, id);
                show(0, net, id, IP, TCP);
                // clitTCP(&tcpV, IP, TCP);
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
    }

    // check if server socket is ready for accepting new connections
    if (FD_ISSET(server_fd, &fds))
    {
        addrlen = sizeof(addr);
        if ((newfd = accept(fd, &addr, &addrlen)) == -1)
            /*error*/ exit(1);
        while ((n = read(newfd, buffer, 128)) != 0)
        {
            if (n == -1) /*error*/
                exit(1);
            ptr = &buffer[0];
            while (n > 0)
            {
                if ((nw = write(newfd, ptr, n)) <= 0) /*error*/
                    exit(1);
                n -= nw;
                ptr += nw;
            }
        }
        close(newfd);
    }

    /*// if (max_clients < 10)
    //{
    if ((client_fds[max_clients] = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    printf("New client connected: ");

    // add new client socket to set
    FD_SET(client_fds[max_clients], &fds);
    max_clients++;
    /*}
    else
    {
        printf("Max clients reached\n");
    }*/
}

// check if any client socket is ready for reading
for (int i = 0; i < max_clients; i++)
{
    if (FD_ISSET(client_fds[i], &fds))
    {
        int valread = read(client_fds[i], buffer, sizeof(buffer));
        if (valread == 0)
        {
            // client disconnected
            close(client_fds[i]);
            FD_CLR(client_fds[i], &fds);
            printf("Client disconnected\n");
            max_clients--;
            client_fds[i] = client_fds[max_clients];
        }
        else
        {
            // process client message
            printf("Client message: %s", buffer);
        }
    }
}
}