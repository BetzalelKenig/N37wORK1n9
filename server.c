#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <jansson.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[])
{
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    char buffer[BUFFER_SIZE];
    char *response_template = "HTTP/1.1 200 OK\nContent-Type: application/json\n\n%s\n";

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

        /* Read the request from the client */
        memset(buffer, 0, BUFFER_SIZE);
        if (recv(client_fd, buffer, BUFFER_SIZE - 1, 0) < 0) {
            perror("Error receiving request");
            return 1;
        }

        /* Check if the request is a POST request */
        if (strncmp(buffer, "POST / HTTP/1.1", 15) == 0) {
            /* Parse the request body as JSON */
            char *body = strstr(buffer, "\r\n\r\n");
            if (body == NULL) {
                perror("Error finding request body");
                return 1;
            }
            body += 4;

        json_error_t error;
        json_t *root = json_loads(body, 0, &error);
        if (root == NULL) {
            fprintf(stderr, "Error parsing JSON: %s\n", error.text);
            return 1;
        }

        /* Modify the request data */
        json_t *name = json_object_get(root, "name");
        if (name != NULL && json_is_string(name)) {
            char *name_str = strdup(json_string_value(name));
            for (int i = 0; name_str[i] != '\0'; i++) {
                name_str[i] = toupper(name_str[i]);
            }
        }

        /* Build the response */
        json_t *response = json_pack("{s:s, s:i}", "message", "Hello!", "success", 1);

        /* Serialize the response to a JSON string */
        char *response_str = json_dumps(response, 0);

        /* Send the response to the client */
        char response_buffer[BUFFER_SIZE];
        snprintf(response_buffer, BUFFER_SIZE, response_template, response_str);
        if (send(client_fd, response_buffer, strlen(response_buffer), 0) < 0) {
            perror("Error sending response");
            return 1;
        }

        /* Clean up */
        json_decref(root);
        json_decref(response);
        free(response_str);
    }

    /* Close the client socket */
    close(client_fd);
}

return 0;
}