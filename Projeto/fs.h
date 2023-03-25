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
    int tabExp[100]; // 00-99
    // char id[3];
};

void create(char *name);
void delete(char *name);
void showNames();
void get(char *dest, char *id, char *name);
void showTopo();
void showRouting();
void query(char *destR, char *origR, char *nameR, int fdR, char *id);
void CNContent(int CNC, char *destR, char *origR, char *nameR, int fdR, char *id);

#endif