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

struct stream_socket {
    int fd;
};

auto get_a_stream_socket() {
    stream_socket sock{};
    sock.fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock.fd > 0)
        return sock;
    error("ERROR opening socket.");
    return stream_socket{};
}

auto bind_stream_socket_to_any(stream_socket sock, int portno) {
    struct sockaddr_in serv_addr;
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sock.fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding.");
}

auto accept_a_client(stream_socket sock) {
    struct sockaddr_in cli_addr;
    auto clilen = sizeof(cli_addr);
    stream_socket newsock{};
    newsock.fd = accept(sock.fd, (struct sockaddr *) &cli_addr, (socklen_t *) &clilen);
    if (newsock.fd < 0)
        error("ERROR on accept.");
    return newsock;
}

auto read_from_socket(stream_socket sock, char* buffer, int len) {
    bzero(buffer, len);
    auto n = read(sock.fd, buffer, len - 1);
    if (n < 0)
        error("ERROR reading from socket.");
}

auto write_to_socket(stream_socket sock, const char* buffer, int len) {
    auto n = write(sock.fd, buffer, len);
    if (n < 0)
        error("ERROR writing to socket.");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "ERROR, no port provided.\n");
        exit(1);
    }

    auto sock = get_a_stream_socket();

    bind_stream_socket_to_any(sock, atoi(argv[1]));

    listen(sock.fd, 5) ;

    auto newsock = accept_a_client(sock);
    
    char buffer[256];
    read_from_socket(newsock, buffer, 256);
    printf("Here is the message: %s\n", buffer);

    write_to_socket(newsock, "I got your message", 18);

    return 0;
}