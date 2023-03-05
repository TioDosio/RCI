#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>

int main(int argc, char *argv[])
{
    char *IP = NULL;    // IP do TCP que é dado
    int TCP = 0;        // Porto do TCP que é dado
    char *regIP = NULL; // IP do UDP pode ou não ser dado
    int regUDP = 0;     // Porto do UDP que pode ou não ser dado
    switch (argc)
    {
    case 1:
        printf("Sem argumentos\n"); // Não tem argumentos
        break;
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
        break;
    }
}