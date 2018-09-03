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

#include <iostream>
#include <string>
#include <array>

#include "argh.h"
#include "exceptions.h"

auto get_server_address(std::string name, int portno) {
    auto server = gethostbyname(name.c_str());
    if (server == NULL) {
        throw std::runtime_error{ "No such host"};
    }

    struct sockaddr_in serv_addr;
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *) server->h_addr, (char*)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    return serv_addr;
}

struct client {
    client(std::string address, int port) {
        fd_ = socket(AF_INET, SOCK_STREAM, 0);
        if (fd_ < 0)
            error();
        connect(get_server_address(address, port));
    }

    template<typename Container>
    auto write(const Container& buffer) {
        auto n = ::write(fd_, buffer.data(), buffer.size());
        if (n < 0)
            error();
        return n;
    }

    template<typename Container>
    auto read(Container& buffer) {
        auto n = ::read(fd_, buffer.data(), buffer.size() - 1);
        if (n < 0)
            error();
        return n;
    }

private:
    void connect(struct sockaddr_in serv_addr) {
        if (::connect(fd_, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
            error();
    }

    int fd_;
};

auto gap_with_server(client& clnt) {
    std::cout << "Please enter the message: ";
    std::string message;
    std::cin >> message;

    clnt.write(message);

    std::array<char, 256> buffer{};
    clnt.read(buffer);

    std::cout << buffer.data() << std::endl;

    if (message == "bye" || message == "shutdown")
        return false;

    return true;
}

int main(int argc, char *argv[]) {
    try {
        auto cmd_line = argh::parser{ argv };
        std::string address{};
        cmd_line({ "-s", "--server" }, "localhost") >> address;
        int portno{};
        cmd_line({ "-p", "--port" }, 9900) >> portno;

        auto clnt = client{ address, portno };
        
        while (gap_with_server(clnt)) {
        }
    }
    catch(std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}