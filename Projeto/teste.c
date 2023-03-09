#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUF_SIZE 1024

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
    if  inet_aton(server_ip, &server_addr.sin_addr) == 0)   ///////////////////////////////////////////////////////////////////////                                         
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

    // Close the socket
    close(sock);
}

int main()
{
    communicate_with_udp_server("193.136.138.142", 59000);
    return 0;
}
