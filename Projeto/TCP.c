#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include "UDP.h"
#include "TCP.h"
#include "fs.h"

extern struct NO node; // ter variavel global em varios ficheiros
extern char TCP[50];   // ter variavel global em varios ficheiros

void client_tcp()
{
    int fdclit = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    if ((fdclit = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return;
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(node.vizExt.Portv));

    if (inet_pton(AF_INET, node.vizExt.IPv, &serv_addr.sin_addr) <= 0)
    {
        printf("IP é: %s", node.vizExt.IPv);
        printf("\nInvalid address/ Address not supported \n");
        return;
    }
    printf("IP é: %s", node.vizExt.Portv);
    if (connect(fdclit, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("IP é: %s", node.vizExt.IPv);
        printf("\nConnection Failed \n");
        return;
    }

    while (1)
    {
        printf("Enter message : ");
        fgets(buffer, 1024, stdin);
        send(fdclit, buffer, strlen(buffer), 0);
        printf("Message sent\n");
    }
}
