#include <socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int socket_create_server(void)
{
    int serverfd, newsock, valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char hello[] = "Hello from server";
    int opt = 1;

    if ((serverfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf("Failed to create server socket\n");
        exit(1);
    }
    printf("Socket Created\n");

    if (setsockopt(serverfd, SOL_SOCKET, 
                    SO_REUSEADDR | SO_REUSEPORT, 
                    &opt, sizeof(opt)))
    {
        printf("Failed to force socket to port\n");
        exit(2);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    printf("Socket forced to 8080\n");

    if (bind(serverfd, (struct sockaddr *)&address, sizeof(address)) == -1)
    {
        printf("Bind failed\n");
        exit(3);
    }
    printf("Socket bind successful\n");
    printf("Listening for connections.....\n");

    if (listen(serverfd, 3) == -1)
    {
        printf("Listen Failed\n");
        exit(4);
    }

    if ((newsock = accept(serverfd, 
                        (struct sockaddr *)&address, 
                        (socklen_t *)&addrlen)) == -1)
    {
        printf("Accept failure\n");
        exit(5);
    }
    printf("Connection accepted\n");

    valread = read(serverfd, buffer, 1024);
    printf("%s\n", buffer);
    send(serverfd, hello, strlen(hello), 0);
    printf("Message Sent\n");

    close(newsock);
    shutdown(serverfd, SHUT_RDWR);
}