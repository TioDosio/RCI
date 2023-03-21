#ifndef functions
#define functions
#include "fs.h"

struct infoNO
{
    char IDv[3];
    char IPv[20];
    char Portv[6];
};
struct NO
{
    struct infoNO vizBackup;
    struct infoNO vizExt;
    struct infoNO vizInt[98];
    char names[20][100];
    int flagVaz;
};

void create(char *name, int flagName);
void delete(char *name, int flagName);
void showNames(int flagName);
void get(char *dest, char *name);
void showTopo(int maxclits);

#endif
