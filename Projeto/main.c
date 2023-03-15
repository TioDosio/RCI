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

typedef struct UDP
{
    char IDv[3];
    char IPv[20];
    char Portv[6];
} UDP;

UDP ban;
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
void Com_TCP()
{
}

void djoin(char *net, char *id, char *bootid, char *bootIP, char *bootTCP) //                    djoin(net, id, bootid, bootIP, bootTCP);

{
    printf("djoining...\n");
}
void sair()
{
    printf("saíndo...\n");
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
    int fd0;
    fd0 = fileno(stdin);
    if (fd0 == -1)
    {
        perror("fileno");
        exit(EXIT_FAILURE);
    }
    char *IP = NULL;     // IP do TCP que é dado
    char *TCP = NULL;    // Porto do TCP que é dado
    char *regIP = NULL;  // IP do UDP pode ou não ser dado
    char *regUDP = NULL; // Porto do UDP que pode ou não ser dado
    switch (argc)
    {
    case 1:
        printf("Sem argumentos\n"); // Não tem argumentos
        exit(1);
    case 3: // Recebe apenas o IP e o TCP
        printf("Com 2 argumentos\n");
        regIP = "193.136.138.142";
        regUDP = "59000";
        IP = "95.95.75.236"; // argv[1];
        TCP = argv[2];       //"59001";       // atoi(argv[2]);
        printf("IP: %s\n", IP);
        printf("TCP: %s\n", TCP);
        printf("RegIp: %s\n", regIP);
        printf("RegUDP: %s\n", regUDP);
        break;
    case 5: // Recebe o IP e o TCP e o IP e o UDP
        printf("Com 4 argumentos\n");
        IP = argv[1];
        regIP = argv[3];
        // TCP = atoi(argv[2]);
        // regUDP = atoi(argv[4]);
        printf("IP: %s\n", IP);
        printf("TCP: %s\n", TCP);
        printf("RegIp: %s\n", regIP);
        printf("RegUDP: %s\n", regUDP);
        break;
    default:
        printf("Argumentos invalidos\n");
        printf("argc %d\n", argc);
        help();
    }
    while (1)
    {
        // for com fdset com todos os fd's
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        char buf[100];
        int rval = select(2, &readfds, NULL, NULL, NULL);
        if (rval == -1)
        {
            perror("select");
            printf("Error rval a -1\n");
            exit(EXIT_FAILURE);
        }
        fgets(buf, sizeof(buf), stdin);
        char strV[10]; // guardar o 1º comando
        sscanf(buf, "%s ", strV);
        if (FD_ISSET(STDIN_FILENO, &readfds))
        {
            if (strcmp(strV, "join") == 0) // join net id
            {
                char id[3], net[4];
                sscanf(buf, "%s %s %s", strV, net, id);
                reg(net, id, IP, TCP);
            }
            else if (strcmp(strV, "djoin") == 0) // djoin net id bootid bootIP bootTCP
            {
                char id[3], net[4], bootid[3], bootIP[16], bootTCP[5];
                sscanf(buf, "%s %s %s %s %s %s", strV, net, id, bootid, bootIP, bootTCP);
                djoin(net, id, bootid, bootIP, bootTCP);
            }
            else if (strcmp(strV, "create") == 0) //////////////////////////////////////////// create name -> Criar vários nomes
            {
                char name[101];
                sscanf(buf, "%s %s", strV, name);
                printf("created name: %s\n", name);
            }
            else if (strcmp(strV, "delete") == 0) // delete name
            {
                // char name[] = "";
            }
            else if (strcmp(strV, "get") == 0) // get dest name
            {
                char dest[4], name[101];
                sscanf(buf, "%s %s %s", strV, dest, name);
                get(dest, name);
            }
            else if ((strcmp(strV, "show topology") == 0) || (strcmp(strV, "st\n") == 0)) // show topology (st)
            {
                st();
            }
            else if ((strcmp(strV, "show names") == 0) || (strcmp(strV, "sn\n") == 0)) // show names (sn)
            {
                sn();
            }
            else if ((strcmp(strV, "show routing") == 0) || (strcmp(strV, "sr\n") == 0)) // show routing (sr)
            {
                sr();
            }
            else if (strcmp(strV, "leave") == 0) // leave
            {
                char id[3], net[4];
                sscanf(buf, "%s %s %s", strV, net, id);
                unreg(net, id, IP, TCP);
            }
            else if (strcmp(strV, "show") == 0) // exit
            {
                char id[3], net[4];
                sscanf(buf, "%s %s %s", strV, net, id);
                show(0, net, id, IP, TCP);
                clitTCP(IP, TCP);
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
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
    }
    return 0;
}