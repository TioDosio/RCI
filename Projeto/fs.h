#ifndef functions
#define functions
#include "fs.h"

struct infoNO
{
    char IDv[3];
    char IPv[20];
    char Portv[6];
    int fd;
    char ctrbuf[100];
    int ctrbufsize;
};
struct NO
{
    struct infoNO vizBackup;
    struct infoNO vizExt;
    struct infoNO vizInt[98];
    char names[20][100];
    int flagVaz, flagName, maxInter;
};

void create(char *name);
void delete(char *name);
void showNames();
void get(char *dest, char *id, char *name);
void showTopo();

#endif