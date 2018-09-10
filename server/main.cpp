/*
* This sample is based on source code from this tutorial:
*   http://www.cs.rpi.edu/~moorthy/Courses/os98/Pgms/socket.html
*/

#include <iostream>
#include <array>
#include <string>
#include <ctime>
#include <thread>
#include <functional>

#include <boost/asio.hpp>

#include "argh.h"

using boost::asio::ip::tcp;

namespace gap {
namespace server {

struct client {
    explicit client(tcp::socket sock) :
        sock_ { std::move(sock) }, authenticated_{ false } {
    }

    template<typename Container>
    auto write(const Container& buffer) {
        return boost::asio::write(sock_, boost::asio::buffer(buffer));
    }

    template<typename Container>
    auto read(Container& buffer) {
        using namespace std::placeholders;
        boost::system::error_code ec;
        auto n = boost::asio::read(sock_, boost::asio::buffer(buffer),
            std::bind(&client::read_complete, this, buffer.data(), _1, _2), ec);
        if (!ec || ec == boost::asio::error::eof)
            return n; // Connection closed cleanly by peer.
        else
            throw boost::system::system_error(ec);
    }

    bool is_authenticated() {
        return authenticated_;
    }

    void set_authenticated() {
        authenticated_ = true;
    }

private:
    size_t read_complete(const char* buf, const boost::system::error_code& err, size_t bytes) {
        if (err)
            return 0;
        bool found = std::find(buf, buf + bytes, '\n') < buf + bytes;
        return found ? 0 : 1;
    }

    tcp::socket sock_;
    bool authenticated_;
};

struct server {
    server(boost::asio::io_service& io_service, unsigned short portno) :
        io_service_ { io_service },
        acceptor_ { io_service, tcp::endpoint{ tcp::v4(), portno } } {
    }

    client next_client() {
        tcp::socket sock(io_service_);
        acceptor_.accept(sock);
        return client{ std::move(sock) };
    }

private:
    boost::asio::io_service& io_service_;
    tcp::acceptor acceptor_;
};

using command_action = std::function<void(client&)>;

void default_action(client& clnt, std::string message) {
    clnt.write(message);
}

void shutdown_action(client& clnt, std::string message) {
    default_action(clnt, message);
    std::exit(0);
}

void login_action(client& clnt, std::string message) {
    default_action(clnt, message);
    clnt.set_authenticated();
}

command_action get_command_reply(std::string command) {
    using namespace std::placeholders;
    if (command == "login") {
        return std::bind(login_action, _1, std::string{ "Hello!\n" });
    }
    if (command == "bye") {
        return std::bind(default_action, _1, std::string{ "See you soon...\n" });
    }
    if (command == "shutdown") {
        return std::bind(shutdown_action, _1, std::string{ "Have a nice day...\n" });
    }
    if (command == "time") {
        auto now = std::time(nullptr);
        return std::bind(default_action, _1, std::string{ std::ctime(&now) }.append("\n"));
    }
    return std::bind(default_action, _1, std::string{ "Unknow command, but no problem.\n" });
}

void gap_with_client(client clnt, int clnt_no) {
    while (true) {
        std::cout << "Server: Wait for next command from clint " << clnt_no << "..." << std::endl;
        auto buffer = std::array<char, 256>{};
        if (clnt.read(buffer) <= 0) {
            std::cout << "Server: Client " << clnt_no << " is down.\n" << std::endl;
            return;
        }

        auto command = std::string{ buffer.data() };
        std::cout << "Client " << clnt_no << " says: " << command << std::endl;
        command.pop_back(); // remove trailing \n

        if (!clnt.is_authenticated()) {
            default_action(clnt, "you are not authenticated yet.\n");
            continue;
        }

        auto action = get_command_reply(command);
        action(clnt);
    };
}

}
}

int main(int argc, char *argv[]) {
    auto cmd_line = argh::parser{ argv };
    unsigned short portno{};
    cmd_line({ "-p", "--port" }, 9900) >> portno;

    try {
        boost::asio::io_service io_service;

        auto srv = gap::server::server{ io_service, portno };

        std::cout << "Server: gap started listening on port: " << portno << std::endl;

        auto clnt_no = 0;
        while (true) {
            clnt_no++;
            std::thread{ gap::server::gap_with_client, std::move(srv.next_client()), clnt_no }.detach();
        }
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
