#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

typedef struct node
{
int id; 
int ext;
char name[100][50];
} node;

int main(){
node joaquim;
int i = 0, j = 0;
char camboja [30];
while(i<=100){
    strcpy(joaquim.name[i],"\0");
    i = i + 1;
}
i = 0;
while (i<34)
    {
        printf("%d ", i);
        scanf("%s\n", camboja);
        while ((strcmp(joaquim.name[j], "\0") != 0)) {
           j = j + 1;
    }
    strcpy(joaquim.name[j], camboja);
    j = j + 1;
    i = i + 1;
}
    
    printf("------------------BINDEPAKIPA--------------\n");
    for (i=0; i<34; i++)
    {
     printf("%d %s\n", i, joaquim.name[i]);      
    }

}
//função para join

