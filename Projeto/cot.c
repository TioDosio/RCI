#include <stdio.h>

int main(int argc, char argv[])
{
    char IP;
    char TCP;
    char regIP;
    int regUDP;
    switch (argc)
    {
    case 1:
        printf("Sem argumentos\n");
        break;

    case 2:
        regIP = "193.136.138.142";
        regUDP = 59000;
        printf("RegIp: %s\n", regIP);
        printf("RegUDP: %d\n", regUDP);
        IP = argv[1];
        TCP = argv[2];
        printf("IP: %s\n", IP);
        printf("TCP: %d\n", TCP);
    case 4:

        printf("RegIp: %s\n", regIP);
        printf("RegUDP: %d\n", regUDP);
        IP = argv[1];
        TCP = arg[2];
        printf("IP: %s\n", IP);
        printf("TCP: %d\n", TCP);
    }
}