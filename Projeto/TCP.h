#ifndef TCP_H
#define TCP_H

#include "TCP.h"
typedef struct TCPS
{
    int fdArray[100];
    int flagNFD;
} TCPS;
void client_tcp();
// void clitTCP(TCPS *tcpV, char *IP, char *TCP);
// void servTCP(TCPS *tcpV, char *TCP);

#endif
