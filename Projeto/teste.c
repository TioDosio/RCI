void Show(char *net, char *id, char *IP, char *TCP)
{
    char sendS[10];                  // String a enviar NODES 000
    sprintf(sendS, "NODES %s", net); // NODES net

    printf("sending: %s\n\n", sendS);
    struct addrinfo hints, *res;
    int fd, errcode;
    ssize_t n;
    struct sockaddr addr;
    ssize_t nbytes, nleft, nwritten, nread;
    char *ptr, buffer[2500];
    int socket(int domain, int type, int protocol);
    ssize_t sendto(int s, const void *buf, size_t len, int flags,
                   const struct sockaddr *dest_addr, socklen_t addrlen);
    socklen_t addrlen;
    struct addrinfo
    {                             // (item in a linked list)
        int ai_flags;             // additional options
        int ai_family;            // address family
        int ai_socktype;          // socket type
        int ai_protocol;          // protocol
        socklen_t ai_addrlen;     // address length (bytes)
        struct sockaddr *ai_addr; // socket address
        char *ai_canonname;       // canonical hostname
        struct addrinfo *ai_next; // next item
    };
    fd = socket(AF_INET, SOCK_DGRAM, 0); // UDP socket
    if (fd == -1)                        /*error*/
        exit(1);
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;      // IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP socket
    errcode = getaddrinfo("tejo.tecnico.ulisboa.pt", "59000", &hints, &res);
    if (errcode != 0) /*error*/
        exit(1);
    n = sendto(fd, sendS, strlen(sendS), 0, res->ai_addr, res->ai_addrlen);
    if (n == -1) /*error*/
        exit(1);
    addrlen = sizeof(addr);

    n = recvfrom(fd, buffer, 2500, 0, &addr, &addrlen); // Recebe a resposta do servidor
    if (n == -1)
    { /*error*/
        exit(1);
    }
    buffer[n] = '\0'; // adiciona terminador de string
    printf("received: %s\n\n", buffer);
    char *saveptr;
    char *line = strtok_r(buffer, "\n", &saveptr);
    if (line == NULL)
    {
        printf("No nodes found\n");
    }
    char a[10], b[20], c[10];
    while (line != NULL)
    {
        sscanf(line, "%s %s %s", a, b, c);
        line = strtok_r(NULL, "\n", &saveptr);
    }
    // meter argumentos do ultimo nó na struct
    strcpy(ban.IDv, a);
    strcpy(ban.IPv, b);
    strcpy(ban.Portv, c);

    /* debug para saber que o nó ta a ser lido
    if (line == NULL)
    {
        printf("\n%s %s %s\n", a, b, c);
        printf("\n%s %s %s\n", &ban.IDv, &ban.IPv, &ban.Portv);
    }*/
    close(fd);
    freeaddrinfo(res);
}