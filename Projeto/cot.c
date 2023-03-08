#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>

void help()
{
    printf("Usage: ./cot <IP> <TCP> <regIP> <regUDP>\n");
    printf("Usage: ./cot <IP> <TCP>\n");
    printf("Usage: ./cot\n");
    exit(1);
}
void create(char *name)
{
    printf("creating\n");
}
void delete()
{
    printf("deleting\n");
}
void get(char *dest, char *name)
{
    printf("getting\n");
}
void join(char *net, char *id)
{
    printf("joining... net:%s id:%s\n", net, id);
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

void leave()
{
    printf("leaving...\n");
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
    char *IP = NULL;    // IP do TCP que é dado
    int TCP = 0;        // Porto do TCP que é dado
    char *regIP = NULL; // IP do UDP pode ou não ser dado
    int regUDP = 0;     // Porto do UDP que pode ou não ser dado
    struct timeval tv;
    switch (argc)
    {
    case 1:
        printf("Sem argumentos\n"); // Não tem argumentos
        exit(1);
    case 3: // Recebe apenas o IP e o TCP
        printf("Com 2 argumentos\n");
        regIP = "193.136.138.142";
        regUDP = 59000;
        IP = argv[1];
        TCP = atoi(argv[2]);
        printf("IP: %s\n", IP);
        printf("TCP: %d\n", TCP);
        printf("RegIp: %s\n", regIP);
        printf("RegUDP: %d\n", regUDP);
        break;
    case 5: // Recebe o IP e o TCP e o IP e o UDP
        printf("Com 4 argumentos\n");
        IP = argv[1];
        regIP = argv[3];
        TCP = atoi(argv[2]);
        regUDP = atoi(argv[4]);
        printf("IP: %s\n", IP);
        printf("TCP: %d\n", TCP);
        printf("RegIp: %s\n", regIP);
        printf("RegUDP: %d\n", regUDP);
        break;
    default:
        printf("Argumentos invalidos\n");
        printf("argc %d\n", argc);
        help();
    }

    while (1)
    { // WTF is that?
        fd_set readfds, ready_sockets, current_sockets;
        FD_ZERO(&readfds);
        FD_SET(fd0, &readfds);
        tv.tv_sec = 30;
        tv.tv_usec = 0;
        int rval = select(2, &readfds, NULL, NULL, &tv);
        char buf[100];
        if (rval == -1)
        {
            perror("select");
            printf("Error rval a -1\n");
            exit(EXIT_FAILURE);
        }
        else if (rval > 0)
        {
            if (FD_ISSET(fd0, &readfds))
            {
                fgets(buf, sizeof(buf), stdin);
                char strV[10]; // guardar o 1º comando
                sscanf(buf, "%s ", strV);
                if (strcmp(strV, "join") == 0) // join net id
                {
                    char id[3], net[4];
                    sscanf(buf, "%s %s %s", strV, net, id);
                    join(net, id);
                }
                else if (strcmp(strV, "djoin") == 0) // djoin net id bootid bootIP bootTCP
                {
                    char id[3], net[4], bootid[3], bootIP[16], bootTCP[5];
                    sscanf(buf, "%s %s %s %s %s %s", strV, net, id, bootid, bootIP, bootTCP);
                    djoin(net, id, bootid, bootIP, bootTCP);
                }
                else if (strcmp(strV, "create") == 0) // create name
                {
                    char name[101];
                    sscanf(buf, "%s %s", strV, name);
                    create(name);
                }
                else if (strcmp(strV, "delete") == 0) // delete name
                {
                    char name[101];
                    sscanf(buf, "%s %s", strV, name);
                    delete (name);
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
                    leave();
                }
                else if (strcmp(strV, "exit") == 0) // exit
                {
                    sair();
                }
                else
                {
                    printf("Invalid command\n");
                }
            }
        }
    }
    return 0;
}