#ifndef UDP_H
#define UDP_H
#include "TCP.h"
#include "UDP.h"

int reg(char *net, char *id, char *IP, char *TCP);
void unreg(char *net, char *id, char *IP, char *TCP);
int show(int flagS, char *net, char *id, char *IP, char *TCP);

#endif