#ifndef TCP_H
#define TCP_H
#include "TCP.h"

int client_tcp(char *id, char *IP, char *TCP);
void djoin(char *net, char *id, char *IP, char *TCP, char *bootID, char *bootIP, char *bootTCP);
void client_tcp_djoin(char *id, char *IP, char *TCP, char *bootID, char *bootIP, char *bootTCP);
void leave(char *net, char *id, char *IP, char *TCP, int *client_fds, int maxclits);

#endif
