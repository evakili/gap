/*
* This sample is based on source code from this tutorial:
*   http://www.cs.rpi.edu/~moorthy/Courses/os98/Pgms/socket.html
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void error(const char* message) {
    perror(message);
    exit(0);
}

auto get_server_address(const char* name, int portno) {
    auto server = gethostbyname(name);
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host.\n");
        exit(0);
    }

    struct sockaddr_in serv_addr;
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *) server->h_addr, (char*)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    return serv_addr;
}

struct client {
    client(const char* address, int port) {
        fd_ = socket(AF_INET, SOCK_STREAM, 0);
        if (fd_ < 0)
            error("ERROR opening socket.");
        connect(get_server_address(address, port));
    }

    auto write(const char* buffer) {
        auto n = ::write(fd_, buffer, strlen(buffer));
        if (n < 0)
            error("ERROR writing to socket.");
        return n;
    }

    auto read(char* buffer, size_t length) {
        auto n = ::read(fd_, buffer, length - 1);
        if (n < 0)
            error("ERROR reading from socket.");
        return n;
    }

private:
    void connect(struct sockaddr_in serv_addr) {
        if (::connect(fd_, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
            error("ERROR connecting.");
    }

    int fd_;
};

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }

    auto clnt = client{ argv[1], atoi(argv[2]) };
    
    printf("Please enter the message: ");

    char buffer[256];
    bzero(buffer, 256);
    fgets(buffer, 255, stdin);

    buffer[strlen(buffer) - 1] = '\0'; // remove \n from input
    clnt.write(buffer);

    bzero(buffer, 256);
    clnt.read(buffer, 256);
    printf("%s\n", buffer);
    return 0;
}