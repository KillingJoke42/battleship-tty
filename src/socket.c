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
    int socket_desc, sockaddr_sz, new_socket, yes;
    struct sockaddr_in server, client;
    char *message = "Hello, Client!\n";

    socket_desc = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_desc == -1)
    {
        printf("ERROR: Unable to create socket\n");
        exit(1);
    }

    if (setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, &yes,
                    sizeof(yes)) < 0)
    {
        printf("ERROR: Unable to make socket reusable\n");
        exit(1);
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8888 );

    if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        printf("ERROR: Bind Failed\n");
        exit(1);
    }

    printf("Waiting for incoming connections....");

    listen(socket_desc, 3);

    sockaddr_sz = sizeof(struct sockaddr_in);
    new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&sockaddr_sz);
    if (new_socket < 0)
    {
        printf("ERROR: Accept connection Failed\n");
        exit(1);
    }

    printf("Connection Accpeted\n");
    printf("Received connection from %s at port %d\n", 
            inet_ntoa(client.sin_addr), ntohs(client.sin_port));

    write(new_socket, message, strlen(message));
    close(new_socket);
    close(socket_desc);
    return 0;
}

int socket_create_client(void)
{
    int socket_desc;
    struct sockaddr_in server, client;
    char server_reply[2048];

    if ((socket_desc = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("ERROR: Socket Create Failed\n");
        exit(1);
    }

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons( 8888 );

    if (connect(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        printf("ERROR: connect failed\n");
        exit(1);
    }

    printf("Connected!\n");

    if (recv(socket_desc, server_reply, sizeof(server_reply), 0) < 0)
    {
        printf("ERROR: receive failed\n");
        exit(1);
    }
    printf("%s\n", server_reply);
}