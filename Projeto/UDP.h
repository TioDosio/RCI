#ifndef UDP_H
#define UDP_H

#include "UDP.h"

void reg(char *net, char *id, char *IP, char *TCP);
void unreg(char *net, char *id, char *IP, char *TCP);
int show(int flagS, char *net, char *id, char *IP, char *TCP);

#endif