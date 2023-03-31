#ifndef TCP_H
#define TCP_H
#include "TCP.h"

void client_tcp(char *IP, char *TCP);
void djoin(char *net, char *IP, char *TCP, char *bootID, char *bootIP, char *bootTCP);
void leave(char *net, char *IP, char *TCP);

#endif