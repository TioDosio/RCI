#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main()
{
    int Port = 59000; // PORTO
    int server_fd, client_fds[10], max_clients = 0;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    // create TCP server
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(Port); // port 59000

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 5) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // create file descriptors set
    fd_set fds;
    FD_ZERO(&fds);

    // add stdin, server socket to set
    FD_SET(STDIN_FILENO, &fds);
    FD_SET(server_fd, &fds);

    while (1)
    {
        // wait for activity on one of the file descriptors
        int max_fd = (STDIN_FILENO > server_fd) ? STDIN_FILENO : server_fd;
        for (int i = 0; i < max_clients; i++)
        {
            if (client_fds[i] > 0)
            {
                FD_SET(client_fds[i], &fds);
                if (client_fds[i] > max_fd)
                {
                    max_fd = client_fds[i];
                }
            }
        }

        if (select(max_fd + 1, &fds, NULL, NULL, NULL) < 0)
        {
            perror("select");
            exit(EXIT_FAILURE);
        }

        // check if stdin is ready for reading
        if (FD_ISSET(STDIN_FILENO, &fds))
        {
            fgets(buffer, sizeof(buffer), stdin);
            printf("User input: %s", buffer);
        }

        // check if server socket is ready for accepting new connections
        if (FD_ISSET(server_fd, &fds))
        {
            // if (max_clients < 10)
            //{
            if ((client_fds[max_clients] = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            printf("New client connected: %s:%d\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));

            // add new client socket to set
            FD_SET(client_fds[max_clients], &fds);
            max_clients++;
            /*}
            else
            {
                printf("Max clients reached\n");
            }*/
        }

        // check if any client socket is ready for reading
        for (int i = 0; i < max_clients; i++)
        {
            if (FD_ISSET(client_fds[i], &fds))
            {
                int valread = read(client_fds[i], buffer, sizeof(buffer));
                if (valread == 0)
                {
                    // client disconnected
                    close(client_fds[i]);
                    FD_CLR(client_fds[i], &fds);
                    printf("Client disconnected\n");
                    max_clients--;
                    client_fds[i] = client_fds[max_clients];
                }
                else
                {
                    // process client message
                    printf("Client message: %s", buffer);
                }
            }
        }
    }
}