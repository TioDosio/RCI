int Com_UDP(int PauloBranco, char *net, char *id, char *IP, char *TCP)
{
    char sendV[50];
    char ar[]
    if (PauloBranco == 1)
    {
        // sscanf(sendV, "%s %s %s", PauloBranco, net, id);  // UNREG net id
        sprintf(sendV, "UNREG %s %s", net, id); // NODES net
    }
    else if (PauloBranco == 0)
    {
        sprintf(sendV, "REG %s %s %s %s", net, id, IP, TCP); // REG net id IP TCP
    }
    else
    {
        sprintf(sendV, "NODES %s", net); // NODES net
    }
    printf("sending: %s\n\n", sendV);
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
    n = sendto(fd, sendV, strlen(sendV), 0, res->ai_addr, res->ai_addrlen);
    if (n == -1) /*error*/
        exit(1);
    addrlen = sizeof(addr);

    n = recvfrom(fd, buffer, 2500, 0, &addr, &addrlen); // Recebe a resposta do servidor
    if (n == -1)                                        /*error*/
        exit(1);
    buffer[n] = '\0'; // adiciona terminador de string
    printf("received: %s\n\n", buffer);
    // Create arrays of strings to store the first, second, and third arguments
    char IDar[100][25]; // Maximum string length of 20 characters
    char IPar[100][25];
    char Portar[100][25];

    // Extract the first, second, and third arguments from each line using sscanf()
    char *p = strtok(buffer, "\n"); // Tokenize the string by newline characters
    int i = 0;
    // sscanf(p, "%s %s");
    for (int i = 0; p != NULL && i < 3; i++)
    {
        if (i == 0)
        {
            p = strtok(NULL, "\n");
        }
        sscanf(p, "%s %s %s", IDar[i], IPar[i], Portar[i]); // Extract the three arguments as strings
        p = strtok(NULL, "\n");                             // Get the next line
    }
    // Output the contents of the arrays
    printf("First arguments: ");
    for (int i = 0; i < 3; i++)
    {
        printf("%s %s %s", IDar[i], IPar[i], Portar[i]);
    }
    printf("\n");
    close(fd);
    freeaddrinfo(res);
}