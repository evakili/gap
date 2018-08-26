/*
* This sample is based on source code from this tutorial:
*   http://www.cs.rpi.edu/~moorthy/Courses/os98/Pgms/socket.html
*/

#include <cstring>
#include <iostream>
#include <array>
#include <string>
#include <system_error>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void error() {
    throw std::system_error{ errno, std::system_category().default_error_condition(errno).category() };
}

struct client_socket {
    explicit client_socket(int newfd) : fd_ { newfd } {
        if (fd_ < 0)
            error();
    }

    template<typename Container>
    auto read(Container& buffer) {
        auto n = ::read(fd_, buffer.data(), buffer.size() - 1);
        if (n < 0)
            error();
        return n;
    }

    template<typename Container>
    auto write(const Container& buffer) {
        auto n = ::write(fd_, buffer.data(), buffer.size());
        if (n < 0)
            error();
        return n;
    }

private:
    int fd_;
};

struct server {
    server(int portno) {
        fd_ = socket(AF_INET, SOCK_STREAM, 0);
        if (fd_ <= 0)
            error();
        bind_to_any(portno);
    }

    void start() {
        ::listen(fd_, 5) ;
    }

    client_socket next_client() {
        struct sockaddr_in cli_addr;
        auto clilen = sizeof(cli_addr);
        return client_socket{ accept(fd_, (struct sockaddr *) &cli_addr, (socklen_t *) &clilen) };
    }

private:
    int fd_;

    void bind_to_any(int portno) const {
        struct sockaddr_in serv_addr;
        bzero((char *) &serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons(portno);
        if (bind(fd_, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
            error();
    }
};

void gap_with_client(client_socket clnt) {
        auto buffer = std::array<char, 256>{};
        clnt.read(buffer);
        std::cout << "Here is the message: " << buffer.data() << std::endl;

        clnt.write(std::string{ "I got your message" });
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "ERROR, no port provided." << std::endl;
        return 1;
    }

    try {
        auto srv = server{ atoi(argv[1]) };
        srv.start();

        gap_with_client(srv.next_client());
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
