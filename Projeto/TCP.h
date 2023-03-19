#ifndef TCP_H
#define TCP_H

#include "TCP.h"
typedef struct TCPS
{
    int fdArray[100];
    int flagNFD;
} TCPS;

void client_tcp();
void djoin(char *net, char *id, char *bootid, char *bootIP, char *bootTCP); // djoin(net, id, bootid, bootIP, bootTCP);

#endif
