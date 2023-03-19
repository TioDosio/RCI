#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include "UDP.h"
#include "TCP.h"
#include "fs.h"
extern struct NO node;

void create(char *name, int flagName)
{
    strcpy(node.names[flagName], name);
    printf("Posição:%d, name:%s\n", flagName, node.names[flagName]);
}

void delete(char *name, int flagName)
{
    for (int i = 0; i < flagName; i++) // passa por todos os nomes já criados
    {
        if (strcmp(node.names[i], name) == 0) // procura o nome
        {
            strcpy(node.names[i], "\0");       // apaga o nome
            for (int j = i; j < flagName; j++) // shift left
            {
                strcpy(node.names[j], node.names[j + 1]);
            }
        }
    }
}
void showNames(int flagName)
{
    for (int i = 0; i < flagName; i++)
    {
        printf("Posição:%d, name:%s\n", i, node.names[i]);
    }
}
void get(char *dest, char *name)
{
    printf("getting\n");
}