#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    char *response = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n<html><body>Hello, World!</body></html>\n";

    /* Create the server socket */
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Error creating socket");
        return 1;
    }

    /* Initialize the socket structure */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    server_addr.sin_port = htons(8080);

    /* Bind the socket to the address */
    if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        perror("Error binding socket");
        return 1;
    }

    /* Listen for incoming connections */
    if (listen(server_fd, 1) < 0) {
        perror("Error listening for connections");
        return 1;
    }

    printf("Listening on localhost:8080\n");

    while (1) {
        /* Accept a connection from a client */
        client_len = sizeof(client_addr);
        client_fd = accept(server_fd, (struct sockaddr *) &client_addr, &client_len);
        if (client_fd < 0) {
            perror("Error accepting connection");
            return 1;
        }

        printf("Connection accepted from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        /* Send the response */
        if (send(client_fd, response, strlen(response), 0) < 0) {
            perror("Error sending response");
            return 1;
        }

        /* Close the client connection */
        close(client_fd);
    }

    return 0;
}