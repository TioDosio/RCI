#include <stdio.h>

int main(int argc, char *argv[])
{
    switch (argc)
    {
    case 1:
        printf("Sem argumentos\n");
        break;

    case 2:
        char regIP = "193.136.138.142";
        int regUDP = 59000;
        printf("RegIp: %s\n", regIP);
        printf("RegUDP: %d\n", regUDP);
        IP = argc[1];
        TCP = argc[2];
        printf("IP: %s\n", IP);
        printf("TCP: %d\n", TCP);
    case 4:
        char regIP;
        int regUDP;
        printf("RegIp: %s\n", regIP);
        printf("RegUDP: %d\n", regUDP);
        IP = argc[1];
        TCP = argc[2];
        printf("IP: %s\n", IP);
        printf("TCP: %d\n", TCP);
    }
}