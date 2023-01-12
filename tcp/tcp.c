// Here is an example of a basic implementation of a TCP server:

// This is a basic example of a TCP server that listens on a specified port and waits for a client to connect. Once a connection is established, the server reads data from the client and sends a response. This example covers the basic steps of creating a socket, binding to a port, listening for connections, accepting a connection, reading and writing data, and closing the sockets.

// Note that, this is a basic example and it does not handle the different corner cases and errors that can happen in a real-world scenario.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;

    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }

    // create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    // initialize socket structure
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    // bind the host address
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR on binding");
        exit(1);
    }

    // start listening for clients
    listen(sockfd,5);
    clilen = sizeof(cli_addr);

    // accept a connection
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0) {
        perror("ERROR on accept");
        exit(1);
    }

    // read and write to the client
    bzero(buffer,256);
    n = read(newsockfd,buffer,255);
    if (n < 0) {
        perror("ERROR reading from socket");
        exit(1);
    }
    printf("Here is the message: %s\n",buffer);
    n = write(newsockfd,"I got your message",18);
    if (n < 0) {
        perror("ERROR writing to socket");
        exit(1);
    }

    // close the sockets
    close(newsockfd);
    close(sockfd);
    return 0;
}
