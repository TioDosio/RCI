#ifndef TCP_H
#define TCP_H

#include "TCP.h"
typedef struct TCPS
{
    int fdArray[100];
    int flagNFD;
} TCPS;

void client_tcp(char *id, char *IP, char *TCP);
void djoin(char *net, char *id, char *IP, char *TCP, char *bootID, char *bootIP, char *bootTCP);
void client_tcp_djoin(char *id, char *IP, char *TCP, char *bootID, char *bootIP, char *bootTCP);

#endif
