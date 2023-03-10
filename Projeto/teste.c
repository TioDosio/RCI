#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <arpa/inet.h>

#define BUF_SIZE 1024

void communicate_with_udp_server(const char *server_ip, int server_port);

int main()
{
    // Initialize file descriptors set
    fd_set readfds;
    FD_ZERO(&readfds);

    // Add stdin to set
    FD_SET(STDIN_FILENO, &readfds);

    // Set timeout
    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    // Loop until user enters "quit"
    while (1)
    {
        // Wait for activity on stdin
        int ready = select(FD_SETSIZE, &readfds, NULL, NULL, &timeout);
        if (ready == -1)
        {
            perror("select");
            exit(EXIT_FAILURE);
        }

        // Check if user entered input on stdin
        if (FD_ISSET(STDIN_FILENO, &readfds))
        {
            char input[100];
            fgets(input, 100, stdin);

            // Check if user command is "join 001"
            if (strcmp(input, "join 001\n") == 0)
            {
                communicate_with_udp_server("193.136.138.142", 59000);
            }
            else if (strcmp(input, "quit\n") == 0)
            {
                break;
            }
            else
            {
                printf("Invalid command\n");
            }
        }

        // Reset file descriptors set and timeout
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;
    }

    return 0;
}

void communicate_with_udp_server(const char *server_ip, int server_port)
{
    // Create a UDP socket
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Configure the server address
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    if (inet_aton(server_ip, &server_addr.sin_addr) == 0)
    {
        fprintf(stderr, "Invalid address: %s\n", server_ip);
        exit(EXIT_FAILURE);
    }

    // Send a message to the server
    const char *message = "NODES 001";
    int num_sent = sendto(sock, message, strlen(message), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (num_sent == -1)
    {
        perror("sendto");
        exit(EXIT_FAILURE);
    }

    // Receive a response from the server
    char buf[BUF_SIZE];
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int num_recv = recvfrom(sock, buf, BUF_SIZE, 0, (struct sockaddr *)&client_addr, &client_addr_len);
    if (num_recv == -1)
    {
        perror("recvfrom");
        exit(EXIT_FAILURE);
    }
    buf[num_recv] = '\0';

    // Print the response from the server
    printf("Received response from %s:%d: %s\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), buf);
}