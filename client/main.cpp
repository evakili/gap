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

#include <boost/asio.hpp>

#include "argh.h"
#include "exceptions.h"

using boost::asio::ip::tcp;

namespace gap {
namespace client {

struct client {
    client(boost::asio::io_service& io_service, tcp::resolver::iterator ep_it) :
        io_service_{ io_service }, sock_{ io_service } {
        connect(ep_it);
    }

    template<typename Container>
    auto write(const Container& buffer) {
        return boost::asio::write(sock_, boost::asio::buffer(buffer));
    }

    template<typename Container>
    auto read(Container& buffer) {
        using namespace std::placeholders;
        return boost::asio::read(sock_, boost::asio::buffer(buffer),
            std::bind(&client::read_complete, this, buffer.data(), _1, _2));
    }

private:
    size_t read_complete(const char* buf, const boost::system::error_code& err, size_t bytes) {
        if (err)
            return 0;
        bool found = std::find(buf, buf + bytes, '\n') < buf + bytes;
        return found ? 0 : 1;
    }

    void connect(tcp::resolver::iterator ep_it) {
        boost::asio::connect(sock_, ep_it);
    }

    boost::asio::ip::tcp::socket sock_;
    boost::asio::io_service& io_service_;
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

}
}

int main(int argc, char *argv[]) {
    try {
        auto cmd_line = argh::parser{ argv };
        std::string address{};
        cmd_line({ "-s", "--server" }, "localhost") >> address;
        std::string portno{};
        cmd_line({ "-p", "--port" }, "9900") >> portno;

        boost::asio::io_service io_service{};
        auto ep_it = tcp::resolver{ io_service }.resolve({ address, portno });
        auto clnt = gap::client::client{ io_service, ep_it };
        
        while (gap::client::gap_with_server(clnt)) {
        }
    }
    catch(std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}