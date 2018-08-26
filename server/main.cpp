/*
* This sample is based on source code from this tutorial:
*   http://www.cs.rpi.edu/~moorthy/Courses/os98/Pgms/socket.html
*/

#include <cstring>
#include <iostream>
#include <array>
#include <string>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


void error(const char* message) {
    std::cerr << message << ": " << strerror(errno) << "." << std::endl;
    exit(1);
}

struct client_socket {
    explicit client_socket(int newfd) : fd_ { newfd } {
        if (fd_ < 0)
            error("ERROR on accept.");
    }

    template<typename Container>
    auto read(Container& buffer) {
        auto n = ::read(fd_, buffer.data(), buffer.size() - 1);
        if (n < 0)
            error("ERROR reading from socket.");
        return n;
    }

    template<typename Container>
    auto write(const Container& buffer) {
        auto n = ::write(fd_, buffer.data(), buffer.size());
        if (n < 0)
            error("ERROR writing to socket.");
        return n;
    }

private:
    int fd_;
};

struct server_socket {
    server_socket() {
        fd_ = socket(AF_INET, SOCK_STREAM, 0);
        if (fd_ <= 0)
            error("ERROR opening socket.");
    }

    void bind_to_any(int portno) const {
        struct sockaddr_in serv_addr;
        bzero((char *) &serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons(portno);
        if (bind(fd_, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
            error("ERROR on binding.");
    }

    void listen() {
        ::listen(fd_, 5) ;
    }

    client_socket accept_a_client() {
        struct sockaddr_in cli_addr;
        auto clilen = sizeof(cli_addr);
        return client_socket{ accept(fd_, (struct sockaddr *) &cli_addr, (socklen_t *) &clilen) };
    }

private:
    int fd_;
};

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "ERROR, no port provided." << std::endl;
        return 1;
    }

    auto sock = server_socket{};

    sock.bind_to_any(atoi(argv[1]));

    sock.listen();

    auto newsock = sock.accept_a_client();
    
    auto buffer = std::array<char, 256>{};
    newsock.read(buffer);
    std::cout << "Here is the message: " << buffer.data() << std::endl;

    newsock.write(std::string{ "I got your message" });

    return 0;
}
