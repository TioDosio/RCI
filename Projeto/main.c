/* Grupo 47 André Teodósio e Henrique Delfino.*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include "UDP.h"
#include "TCP.h"
#include "fs.h"
struct NO node;
int debug;
int main(int argc, char *argv[])
{
    char IP[20];     // IP que vai ser introduzido
    char TCP[20];    // TCP que vai ser introduzido
    char regIP[20];  // IP do UDP pode ou não ser dado introduzido
    char regUDP[20]; // Porto do UDP que pode ou não ser dado
    switch (argc)    // leitura dos argumentos de entrada
    {
    case 1:
        printf("Inserir argumentos\n"); // Não tem argumentos
        exit(1);
    case 3: // Recebe apenas o IP e o TCP
        strcpy(regIP, "193.136.138.142");
        strcpy(regUDP, "59000");
        strcpy(IP, argv[1]);
        strcpy(TCP, argv[2]);
        break;
    case 5: // Recebe o IP e o TCP e o regIP e o regUDP
        strcpy(IP, argv[1]);
        strcpy(TCP, argv[2]);
        strcpy(regIP, argv[3]);
        strcpy(regUDP, argv[4]);
        break;
    default:
        printf("Argumentos invalidos\n");
    }
    struct addrinfo hints, *res;
    int errcode, server_fd = -2;
    struct sockaddr addr;
    socklen_t addrlen;
    char bufstdin[128];
    fd_set rfds, fds;                                       // criar conjuntos de fds
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) // cria o socket TCP
    {
        printf("erro socket main.c");
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;       // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP socket
    hints.ai_flags = AI_PASSIVE;
    if ((errcode = getaddrinfo(NULL, TCP, &hints, &res)) != 0) // preenche a estrutura res com o endereço do socket
    {
        printf("erro get addrinfo main.c");
        exit(1);
    }
    if (bind(server_fd, res->ai_addr, res->ai_addrlen) == -1) // conecta o socket ao endereço
    {
        printf("erro bind main.c");
        exit(1);
    }
    if (listen(server_fd, 5) == -1) // 5 é o numero de conexões que podem estar em espera
    {
        printf("erro get addrinfo main.c");
        exit(1);
    }
    char strV[10], strVaux[10], net[4]; // strV-guarda o comando; strVaux-guarda o comando auxiliar; net-guarda a rede
    for (int i = 0; i < 98; i++)        // inicializar o array de vizinhos internos a -2
    {
        node.vizInt[i].fd = -2;
    }
    node.vizExt.fd = -2;          // inicializar o vizinho externo a -2
    for (int i = 0; i < 100; i++) // inicializar a Tabela de Expedicao a -2
    {
        node.tabExp[i] = -2;
    }
    strcpy(node.vizExt.IDv, "");
    strcpy(node.vizExt.IPv, "");
    strcpy(node.vizExt.Portv, "");

    while (1)
    {
        FD_ZERO(&rfds);              // limpa o conjunto de fds
        FD_SET(STDIN_FILENO, &rfds); // diz ao select para estar atento ao stdin
        if (server_fd > 0)
        {
            FD_SET(server_fd, &rfds); // diz ao select para estar atento ao server_fd
        }
        int max_fd = (STDIN_FILENO > server_fd) ? STDIN_FILENO : server_fd; // coloca o maior fd no max_fd entre sdtin e o server
        if (node.vizExt.fd > 0)
        {
            FD_SET(node.vizExt.fd, &rfds); // diz ao select para estar atento ao vizinho externo
            max_fd = node.vizExt.fd;       // se houver vizinho externo coloca o fd como o max_fd
        }
        int Nsel = 0;
        for (int i = 0; i < 98; i++)
        {
            if (node.vizInt[i].fd > 0)
            {
                FD_SET(node.vizInt[i].fd, &rfds); // diz ao select para estar atento ao vizinho interno
                if (node.vizInt[i].fd > max_fd)
                {
                    max_fd = node.vizInt[i].fd; // se houver vizinhos internos coloca o fd como o max_fd
                }
            }
        }
        fds = rfds;
        if ((Nsel = select(max_fd + 1, &fds, NULL, NULL, NULL)) < 0) // select para ver se há algo para ler
        {                                                            /*error*/
            perror("error select");
            exit(EXIT_FAILURE);
        }
        for (int counter = 0; counter < Nsel; counter++) // se houver mais que uma coisa para ler ao mesmo tempo o select retorna o Nsel e repete aqui as vezes necessarias
        {
            if (FD_ISSET(STDIN_FILENO, &fds)) // verifica se o stdin está pronto a ser lido
            {
                int tam;
                strcpy(bufstdin, "");
                fgets(bufstdin, 128, stdin);
                sscanf(bufstdin, "%s", strV);
                if (strcmp(strV, "join") == 0) // join net id
                {
                    tam = sscanf(bufstdin, "%s %s %s", strV, net, node.id);
                    if (tam == 3)
                    {
                        reg(net, IP, TCP); // registar o nó na rede e juntar-me a um outro nó se existir
                    }
                    else
                    {
                        printf("Somethings missing\n");
                    }
                }
                else if (strcmp(strV, "djoin") == 0) // djoin net id bootid bootIP bootTCP
                {
                    char bootid[3] = "", bootIP[50] = "", bootTCP[10] = "", dnet[5] = "";
                    tam = sscanf(bufstdin, "%s %s %s %s %s %s", strV, dnet, node.id, bootid, bootIP, bootTCP);
                    if (tam == 6)
                    {
                        djoin(dnet, IP, TCP, bootid, bootIP, bootTCP); // junta-se a um outro nó sem se ligar ao servidor de nós
                    }
                    else
                    {
                        printf("Somethings missing\n");
                    }
                }
                else if (strcmp(strV, "create") == 0) // create name
                {
                    char name[101] = "";
                    sscanf(bufstdin, "%s %s", strV, name);
                    create(name); // cria um conteudo no nó
                }
                else if (strcmp(strV, "delete") == 0) // delete name
                {
                    char name[101];
                    strcpy(name, "");
                    sscanf(bufstdin, "%s %s", strV, name);
                    delet(name); // apaga um conteudo no nó
                }
                else if (strcmp(strV, "get") == 0) // get dest name
                {
                    char dest[4], name[101] = "";
                    tam = sscanf(bufstdin, "%s %s %s", strV, dest, name);
                    if (tam == 3)
                    {
                        get(dest, name); // pede um conteudo a outro nó
                    }
                    else
                    {
                        printf("Somethings missing\n");
                    }
                }
                else if ((strcmp(strV, "st") == 0)) // show topology (st)
                {
                    showTopo(); // mostra a topologia da rede
                }
                else if ((strcmp(strV, "sn") == 0)) // show names (sn)
                {
                    showNames(); // mostra os nomes dos conteudos do nó
                }
                else if ((strcmp(strV, "sr") == 0)) // show routing (sr)
                {
                    showRouting(); // mostra a tabela de expedicao do nó
                }
                else if (strcmp(strV, "show") == 0) // show net
                {
                    tam = sscanf(bufstdin, "%s %s", strV, strVaux);
                    if (tam == 2)
                    {
                        if (strcmp(strVaux, "routing") == 0) // show routing
                        {
                            showRouting(); // mostra a tabela de expedicao do nó
                        }
                        else if (strcmp(strVaux, "topology") == 0) // show topology
                        {
                            showTopo(); // mostra a topologia da rede
                        }
                        else if (strcmp(strVaux, "names") == 0) // show names
                        {
                            showNames(); // mostra os nomes dos conteudos do nó
                        }
                    }
                    else
                    {
                        printf("Show cmd\nAvalable commands: names-topology-routing\n");
                    }
                }
                else if (strcmp(strV, "exit") == 0) // exit
                {
                    close(server_fd);  // fecha o socket do servidor
                    freeaddrinfo(res); // liberta a memoria do addrinfo
                    exit(0);           // sai do programa
                }
                else if (strcmp(strV, "leave") == 0) // leave net id
                {
                    sscanf(bufstdin, "%s %s %s", strV, net, node.id);
                    leave(net, IP, TCP); // sai da rede e fecha o socket do servidor e com todos os vizinhos
                }
                else if (strcmp(strV, "server") == 0) // exit
                {
                    tam = sscanf(bufstdin, "%s %s", strV, net);
                    if (tam == 2)
                    {
                        show(0, net); // mostra os nós da rede ao utilizador
                    }
                    else
                    {
                        printf("Somethings missing\n");
                    }
                }
                else if (strcmp(strV, "cn") == 0) //  clear names
                {
                    for (int i = 0; i < node.flagName; i++)
                    {
                        strcpy(node.names[i], "");
                    }
                }
                else if (strcmp(strV, "cr") == 0) // clear routing
                {
                    for (int i = 0; i < 100; i++)
                    {
                        node.tabExp[i] = -2;
                    }
                }
                else if (strcmp(strV, "do") == 0) // debug on
                {
                    debug = 1;
                    printf("Debug on\n");
                }
                0 else if (strcmp(strV, "df") == 0) // debug off
                {
                    debug = 0;
                    printf("Debug off\n");
                }
                else
                {
                    printf("Invalid command\n");
                }
            }
            for (int i = 0; i < 98; i++)
            {
                if (FD_ISSET(node.vizInt[i].fd, &fds)) // verifica se o vizinho interno está pronto a ser lido
                {
                    int fdR = -2; // file descriptor do vizinho que enviou a mensagem
                    char bufR[200] = "", cmd[20] = "", bufW[100] = "", *ptr, *line;
                    strcpy(node.vizExt.ctrbuf, "");
                    node.vizInt[i].ctrbufsize = read(node.vizInt[i].fd, node.vizInt[i].ctrbuf, 100); // NEW net id\n NEW net id\n
                    strcpy(bufR, node.vizInt[i].ctrbuf);
                    if (bufR[node.vizInt[i].ctrbufsize - 1] == '\n')
                    {
                        line = strtok_r(bufR, "\n", &ptr);
                        while (line != NULL)
                        {
                            sscanf(line, "%s", cmd);
                            if (strcmp(cmd, "NEW") == 0)
                            {
                                if (debug == 1)
                                {
                                    printf("Int R:%s\n", cmd);
                                }
                                sscanf(line, "%s %s %s %s", cmd, node.vizInt[i].IDv, node.vizInt[i].IPv, node.vizInt[i].Portv); // Recebe NEW e guarda como interno
                                sprintf(bufW, "EXTERN %s %s %s\n", node.vizExt.IDv, node.vizExt.IPv, node.vizExt.Portv);        // Coloca seu Externo no buffer
                                write(node.vizInt[i].fd, bufW, strlen(bufW));                                                   // Envia EXTERN para o Interno
                            }
                            else if (strcmp(cmd, "QUERY") == 0) // QUERY DEST ORIG NAME
                            {
                                if (debug == 1)
                                {
                                    printf("Int R:%s\n", cmd);
                                }
                                char destQ[3], origQ[3], nameQ[100];
                                sscanf(line, "%s %s %s %s", cmd, destQ, origQ, nameQ);
                                fdR = node.vizInt[i].fd;                             // file descriptor do vizinho que enviou a mensagem
                                node.tabExp[atoi(origQ)] = atoi(node.vizInt[i].IDv); // O query veio do nó "origem" por este vizinho logo se quiser ir para esse nó vou para este vizinho
                                query(destQ, origQ, nameQ, fdR);                     // envia o query ou em FLOOD ou para o vizinho que está na tabela de expedicao
                            }
                            else if (strcmp(cmd, "CONTENT") == 0) // CONTENT ORIG DEST NAME
                            {
                                if (debug == 1)
                                {
                                    printf("Int R:%s\n", cmd);
                                }
                                char destC[3], origC[3], nameC[100];
                                sscanf(line, "%s %s %s %s", cmd, destC, origC, nameC); // CONTENT ORIG DEST NAME
                                fdR = node.vizInt[i].fd;                               // file descriptor do vizinho que enviou a mensagem
                                node.tabExp[atoi(origC)] = atoi(node.vizInt[i].IDv);   // O query veio do nó "origem" por este vizinho logo se quiser ir para esse nó vou para este vizinho
                                CNContent(0, destC, origC, nameC, fdR);                // Se não somos quem pediu o query, enviamos para o vizinho que está na tabela de expedicao
                            }
                            else if (strcmp(cmd, "NOCONTENT") == 0) // NOCONTENT ORIG DEST NAME
                            {
                                if (debug == 1)
                                {
                                    printf("Int R:%s\n", cmd);
                                }
                                char destC[3], origC[3], nameC[100];
                                sscanf(line, "%s %s %s %s", cmd, destC, origC, nameC);
                                fdR = node.vizInt[i].fd;                             // file descriptor do vizinho que enviou a mensagem
                                node.tabExp[atoi(origC)] = atoi(node.vizInt[i].IDv); // O query veio do nó "origem" por este vizinho logo se quiser ir para esse nó vou para este vizinho
                                CNContent(1, destC, origC, nameC, fdR);              // Se não somos quem pediu o query, enviamos para o vizinho que está na tabela de expedicao
                            }
                            else if (strcmp(cmd, "WITHDRAW") == 0)
                            {
                                if (debug == 1)
                                {
                                    printf("Int R:%s\n", cmd);
                                }
                                char idW[3];             // id do nó que se vai retirar da rede
                                fdR = node.vizInt[i].fd; // file descriptor do vizinho que enviou a mensagem
                                sscanf(line, "%s %s", cmd, idW);
                                wdraw(idW, fdR); // retira o nó da rede
                            }
                            node.vizInt[i].ctrbufsize = 0;
                            line = strtok_r(NULL, "\n", &ptr); // vai buscar a próxima linha
                        }
                    }
                    else if (node.vizInt[i].ctrbufsize == 0) // saída de um Vizinho Interno
                    {
                        char bufW[15] = "";
                        printf("vizInt[%d] disconnected\n", i);
                        sprintf(bufW, "WITHDRAW %s\n", node.vizInt[i].IDv); // WITHDRAW ID
                        node.tabExp[atoi(node.vizInt[i].IDv)] = -2;         // retira o nó da tabela de expedicao
                        for (int i = 0; i < node.maxInter; i++)
                        {
                            if (node.tabExp[i] == atoi(node.vizInt[i].IDv))
                            {
                                node.tabExp[i] = -2; // retira o nó da tabela de expedicao
                            }
                        }
                        node.vizInt[i].fd = -2;                    // retira o nó da lista de vizinhos internos
                        write(node.vizExt.fd, bufW, strlen(bufW)); // envia o WITHDRAW para o vizinho externo
                        for (int j = 0; j < node.maxInter; j++)
                        {
                            if (node.vizInt[j].fd != -2)
                            {
                                write(node.vizInt[j].fd, bufW, strlen(bufW)); // envia o WITHDRAW para os vizinhos internos se houver
                            }
                        }
                        strcpy(node.vizInt[i].IDv, ""); // Limpeza dos dados do vizinho
                        strcpy(node.vizInt[i].IPv, "");
                        strcpy(node.vizInt[i].Portv, "");
                        strcpy(node.vizInt[i].ctrbuf, "");
                        node.vizInt[i].ctrbufsize = 0;
                        for (int j = i; j < node.maxInter; j++)
                        {
                            node.vizInt[j] = node.vizInt[j + 1]; // reorganiza a lista de vizinhos internos
                        }
                        node.maxInter--;
                    }
                }
                FD_CLR(node.vizInt[i].fd, &fds); // retira a flag de estar pronto para leitura
            }
            if (FD_ISSET(node.vizExt.fd, &fds)) // Verifica se o vizinho externo está pronto para leitura
            {
                int fdR = -2;
                char bufR[200] = "", cmd[20] = "", bufW[100] = "", *ptr, *line;
                strcpy(node.vizExt.ctrbuf, "-1");
                node.vizExt.ctrbufsize = read(node.vizExt.fd, node.vizExt.ctrbuf, 100);
                strcpy(bufR, node.vizExt.ctrbuf);
                if (bufR[node.vizExt.ctrbufsize - 1] == '\n')
                {
                    line = strtok_r(bufR, "\n", &ptr); // vai buscar a primeira linha
                    while (line != NULL)
                    {
                        sscanf(line, "%s", cmd); // vai buscar o comando
                        if (strcmp(cmd, "NEW") == 0)
                        {
                            if (debug == 1)
                            {
                                printf("Ext R:%s\n", cmd);
                            }
                            sscanf(line, "%s %s %s %s", cmd, node.vizExt.IDv, node.vizExt.IPv, node.vizExt.Portv);           // guarda os dados do vizinho externo
                            if (strlen(node.vizExt.IDv) != 2 || strlen(node.vizExt.IPv) > 15 || strlen(node.vizExt.IPv) < 7) // verifica se os dados estão corretos
                                sprintf(bufW, "EXTERN %s %s %s\n", node.vizExt.IDv, node.vizExt.IPv, node.vizExt.Portv);
                            write(node.vizExt.fd, bufW, strlen(bufW)); // envia os dados do nosso vizinho externo para o vizinho externo
                        }
                        else if (strcmp(cmd, "EXTERN") == 0)
                        {
                            if (debug == 1)
                            {
                                printf("Ext R:%s\n", cmd);
                            }
                            sscanf(line, "%s %s %s %s", cmd, node.vizBackup.IDv, node.vizBackup.IPv, node.vizBackup.Portv);
                        }
                        else if (strcmp(cmd, "QUERY") == 0) // QUERY DEST ORIG NAME
                        {
                            if (debug == 1)
                            {
                                printf("Ext R:%s\n", cmd);
                            }
                            char destQ[3], origQ[3], nameQ[100];
                            sscanf(line, "%s %s %s %s", cmd, destQ, origQ, nameQ);
                            node.tabExp[atoi(origQ)] = atoi(node.vizExt.IDv); // guarda o ID do vizinho externo na tabela de expedicao
                            fdR = node.vizExt.fd;                             // guarda o file descriptor do vizinho externo
                            query(destQ, origQ, nameQ, fdR);                  // envia o query ou em FLOOD ou para o vizinho que está na tabela de expedicao
                        }
                        else if (strcmp(cmd, "CONTENT") == 0) // CONTENT ORIG DEST NAME
                        {
                            if (debug == 1)
                            {
                                printf("Ext R:%s\n", cmd);
                            }
                            char destC[3], origC[3], nameC[100];
                            sscanf(line, "%s %s %s %s", cmd, destC, origC, nameC);
                            fdR = node.vizExt.fd;                             // guarda o file descriptor do vizinho externo
                            node.tabExp[atoi(origC)] = atoi(node.vizExt.IDv); // guarda o ID do vizinho externo na tabela de expedicao
                            CNContent(0, destC, origC, nameC, fdR);           // Se não somos quem pediu o query, enviamos para o vizinho que está na tabela de expedicao
                        }
                        else if (strcmp(cmd, "NOCONTENT") == 0) // NOCONTENT ORIG DEST NAME
                        {
                            if (debug == 1)
                            {
                                printf("Ext R:%s\n", cmd);
                            }
                            char destC[3], origC[3], nameC[100];
                            sscanf(line, "%s %s %s %s", cmd, destC, origC, nameC);
                            fdR = node.vizExt.fd;                             // guarda o file descriptor do vizinho externo
                            node.tabExp[atoi(origC)] = atoi(node.vizExt.IDv); // guarda o ID do vizinho externo na tabela de expedicao
                            CNContent(1, destC, origC, nameC, fdR);           // Se não somos quem pediu o query, enviamos para o vizinho que está na tabela de expedicao
                        }
                        else if (strcmp(cmd, "WITHDRAW") == 0)
                        {
                            if (debug == 1)
                            {
                                printf("Ext R:%s\n", cmd);
                            }
                            char idW[3], bufsend[100];
                            int fdR = node.vizExt.fd;
                            strcpy(bufsend, "");
                            sscanf(line, "%s %s", cmd, idW); // vai buscar o ID do vizinho que se vai desligar
                            wdraw(idW, fdR);                 // envia o withdraw para todos os vizinhos
                        }
                        strcpy(line, "");
                        line = strtok_r(NULL, "\n", &ptr); // vai buscar a próxima linha
                    }
                    node.vizExt.ctrbufsize = 0;
                }
                else if (node.vizExt.ctrbufsize == 0) // saída do vizinho externo
                {
                    char bufW[100]; // withdraw id
                    printf("Vizinho Externo disconnected\n");
                    sprintf(bufW, "WITHDRAW %s\n", node.vizExt.IDv);
                    node.tabExp[atoi(node.vizExt.IDv)] = -2; // atualizar tabela de expedicao
                    for (int i = 0; i < 100; i++)
                    {
                        if (node.tabExp[i] == atoi(node.vizExt.IDv))
                        {
                            node.tabExp[i] = -2; // atualizar tabela de expedicao
                        }
                    }
                    for (int j = 0; j < node.maxInter; j++)
                    {
                        if (node.vizInt[j].fd != -2)
                        {
                            write(node.vizInt[j].fd, bufW, strlen(bufW)); // envia o withdraw para todos os vizinhos internos
                        }
                    }
                    if (strcmp(node.id, node.vizBackup.IDv) != 0) // não somos ancora
                    {
                        if (debug == 1)
                        {
                            printf("Não somos ancora\n");
                        }
                        strcpy(node.vizExt.IDv, node.vizBackup.IDv); // atualizar vizinho externo com a informação do vizinho backup
                        strcpy(node.vizExt.IPv, node.vizBackup.IPv);
                        strcpy(node.vizExt.Portv, node.vizBackup.Portv);
                        client_tcp(IP, TCP); // ligar ao novo vizinho externo (vizinho backup)
                        for (int i = 0; i < node.maxInter; i++)
                        {
                            if (node.vizInt[i].fd != -2)
                            {
                                strcpy(bufW, "");
                                sprintf(bufW, "EXTERN %s %s %s\n", node.vizExt.IDv, node.vizExt.IPv, node.vizExt.Portv);
                                write(node.vizInt[i].fd, bufW, strlen(bufW)); // envia o novo vizinho externo para todos os vizinhos internos
                            }
                        }
                    }
                    else if (node.maxInter != 0) // somos ancora e temos vizinhos internos
                    {
                        if (debug == 1)
                        {
                            printf("Somos ancora e temos internos\n");
                        }
                        strcpy(node.vizExt.IDv, node.vizInt[0].IDv); // atualizar vizinho externo com a informação do primeiro vizinho interno
                        strcpy(node.vizExt.IPv, node.vizInt[0].IPv);
                        strcpy(node.vizExt.Portv, node.vizInt[0].Portv);
                        node.vizExt.fd = node.vizInt[0].fd;
                        for (int j = 0; j < node.maxInter; j++)
                        {
                            node.vizInt[j] = node.vizInt[j + 1]; // reorganizar os vizinhos internos
                        }
                        node.maxInter--;
                        sprintf(bufW, "EXTERN %s %s %s\n", node.vizExt.IDv, node.vizExt.IPv, node.vizExt.Portv);
                        write(node.vizExt.fd, bufW, strlen(bufW)); // envia o vizinho interno escolhido que agora é ancora e enviamos EXTERN (ele próprio)
                        for (int i = 0; i < node.maxInter; i++)
                        {
                            write(node.vizInt[i].fd, bufW, strlen(bufW)); // envia o novo vizinho externo para todos os vizinhos internos
                        }
                    }
                    else
                    {
                        printf("Não tenho nenhuma ligação\n");
                        strcpy(node.vizExt.IDv, ""); // Limpa informação do vizinho externo
                        strcpy(node.vizExt.IPv, "");
                        strcpy(node.vizExt.Portv, "");
                        strcpy(node.vizBackup.IDv, node.id); // Somos a ancora
                        strcpy(node.vizBackup.IPv, IP);
                        strcpy(node.vizBackup.Portv, TCP);
                        node.flagVaz = 0;
                        node.vizExt.fd = -2;
                    }
                }
                FD_CLR(node.vizExt.fd, &fds);
            }
            if (FD_ISSET(server_fd, &fds)) // Verifica se há nós a tentarem ligar-se
            {
                addrlen = sizeof(addr);
                if (node.flagVaz == 0) // Sou o único da rede e ligam-se a mim
                {
                    if (debug == 1)
                    {
                        printf("Accept Ext\n");
                    }
                    node.vizExt.fd = accept(server_fd, &addr, &addrlen); // aceita a ligação do vizinho externo
                    if ((node.vizExt.fd == -1))
                    {
                        printf("EXT erro accept main.c");
                        exit(1);
                    }
                    node.flagVaz++;
                }
                else if (node.flagVaz > 0) // Já não sou o único da rede e ligam-se a mim
                {
                    if (debug == 1)
                    {
                        printf("Accept Int\n");
                    }
                    node.vizInt[node.maxInter].fd = accept(server_fd, &addr, &addrlen); // aceita a ligação do vizinho interno
                    if (node.vizInt[node.maxInter].fd == -1)
                    {
                        printf("INT erro accept main.c");
                        exit(1);
                    }
                    node.maxInter++; // incrementa o número de vizinhos internos
                }
                FD_CLR(server_fd, &fds);
            }
        }
    }
    return 0;
}