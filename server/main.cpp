/*
* This sample is based on source code from this tutorial:
*   http://www.cs.rpi.edu/~moorthy/Courses/os98/Pgms/socket.html
*/

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char* message) {
    perror(message);
    exit(1);
}

auto get_a_stream_socket() {
    auto sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock > 0)
        return sock;
    error("ERROR opening socket.");
    return -1;
}

auto bind_stream_socket_to_any(int sockfd, int portno) {
    struct sockaddr_in serv_addr;
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding.");
}

auto accept_a_client(int sockfd) {
    struct sockaddr_in cli_addr;
    auto clilen = sizeof(cli_addr);
    auto newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, (socklen_t *) &clilen);
    if (newsockfd < 0)
        error("ERROR on accept.");
    return newsockfd;
}

auto read_from_socket(int sockfd, char* buffer, int len) {
    bzero(buffer, len);
    auto n = read(sockfd, buffer, len - 1);
    if (n < 0)
        error("ERROR reading from socket.");
}

auto write_to_socket(int sockfd, const char* buffer, int len) {
    auto n = write(sockfd, buffer, len);
    if (n < 0)
        error("ERROR writing to socket.");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "ERROR, no port provided.\n");
        exit(1);
    }

    auto sockfd = get_a_stream_socket();

    bind_stream_socket_to_any(sockfd, atoi(argv[1]));

    listen(sockfd, 5) ;

    auto newsockfd = accept_a_client(sockfd);
    
    char buffer[256];
    read_from_socket(newsockfd, buffer, 256);
    printf("Here is the message: %s\n", buffer);

    write_to_socket(newsockfd, "I got your message", 18);

    return 0;
}