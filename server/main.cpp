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
#include <map>
#include <fstream>

#include <boost/asio.hpp>

#include "argh.h"

using boost::asio::ip::tcp;

namespace gap {
namespace server {

using credentials = std::pair<std::string, std::string>;

struct authenticator {
    virtual ~authenticator() = default;

    virtual bool authenticate(const credentials& creds) = 0;
};

struct volatile_authenticator : public authenticator {
    bool authenticate(const credentials& creds) override {
        auto user = users_.find(creds.first);
        if (user != users_.end())
            if (user->second == creds.second)
                return true;
        return false;
    }

private:
    static const std::map<std::string, std::string> users_;
};

const std::map<std::string, std::string> volatile_authenticator::users_ = {
    { "hasan", "123" },
    { "reza", "abc" },
    { "ali", "qaz" }
};

struct persistant_authenticator : public authenticator {
    persistant_authenticator() {
        auto in = std::ifstream{ "/etc/gap/users.dat" };
        auto username = std::string{};
        auto password = std::string{};
        while (in >> username >> password)
            users_.insert(std::make_pair(username, password));
    }

    bool authenticate(const credentials& creds) override {
        auto user = users_.find(creds.first);
        if (user != users_.end())
            if (user->second == creds.second)
                return true;
        return false;
    }

private:
    std::map<std::string, std::string> users_;
};

struct client {
    explicit client(tcp::socket sock, authenticator& auth) :
        sock_ { std::move(sock) },
        authenticated_{ false },
        username_{},
        auth_{ auth } {
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

    std::string username() {
        return username_;
    }

    bool try_login(const credentials& creds) {
        authenticated_ = false;
        if (auth_.authenticate(creds)) {
            username_ = creds.first;
            authenticated_ = true;
        }
        return authenticated_;
    }

    void logoff() {
        username_.clear();
        authenticated_ = false;
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
    std::string username_;
    authenticator& auth_;
};

struct server {
    server(boost::asio::io_service& io_service, unsigned short portno, authenticator& auth) :
        io_service_ { io_service },
        acceptor_ { io_service, tcp::endpoint{ tcp::v4(), portno } },
        auth_{ auth } {
    }

    client next_client() {
        tcp::socket sock(io_service_);
        acceptor_.accept(sock);
        return client{ std::move(sock), auth_ };
    }

private:
    boost::asio::io_service& io_service_;
    tcp::acceptor acceptor_;
    authenticator& auth_;
};

using command_action = std::function<void(client&, std::string)>;

void default_action(client& clnt, std::string params, std::string message) {
    clnt.write(message);
}

void shutdown_action(client& clnt, std::string params) {
    if (clnt.is_authenticated()) {
        default_action(clnt, "", "Have a nice day...\n");
        std::exit(0);
    }
    else {
        clnt.write("You are not authenticated yet.\n");
    }
}

void bye_action(client& clnt, std::string params) {
    default_action(clnt, "", "See you soon...\n");
}

std::pair<std::string, std::string> parse_login_params(std::string params) {
    auto pos = params.find(';');
    auto username = params;
    auto password = std::string{};
    if (pos != std::string::npos) {
        username = params.substr(0, pos);
        password = params.substr(pos + 1);
    }
    return { username, password };
}

void login_action(client& clnt, std::string params) {
    if (params.empty()) {
        default_action(clnt, "", "Please provide username!\n");
    }
    else {
        if (clnt.try_login(parse_login_params(params)))
            default_action(clnt, "", "Hello " + clnt.username() + "!\n");
        else
            default_action(clnt, "", "Invalid username or password!\n");
    }
}

void logoff_action(client& clnt, std::string params) {
    clnt.logoff();
    default_action(clnt, "", "Miss you...\n");
}

void time_action(client& clnt, std::string params) {
    if (clnt.is_authenticated()) {
        auto now = std::time(nullptr);
        default_action(clnt, "", std::string{ std::ctime(&now) });
    } else {
        clnt.write("You are not authenticated yet.\n");
    }
}

const std::map<std::string, command_action> action_map = {
    { "login", login_action },
    { "logoff", logoff_action },
    { "bye", bye_action },
    { "time", time_action },
    { "shutdown", shutdown_action },
};

command_action get_command_reply(std::string command) {
    using namespace std::placeholders;
    auto action = action_map.find(command);
    if (action != action_map.end())
        return action->second;
    return std::bind(default_action, _1, _2, std::string{ "Unknow command, but no problem.\n" });
}

auto parse_command(std::string command) {
    auto paramsPos = command.find(':');
    auto params = std::string{};
    if (paramsPos != std::string::npos) {
        params = command.substr(paramsPos + 1);
        command.erase(paramsPos);
    }
    return std::make_pair(command, params);
}

void gap_with_client(client clnt, int clnt_no) {
    while (true) {
        std::cout << "[Server] Client " << clnt_no << " is connected." << std::endl;
        auto buffer = std::array<char, 256>{};
        if (clnt.read(buffer) <= 0) {
            std::cout << "[Server] Client " << clnt_no << " is down.\n" << std::endl;
            return;
        }

        auto command = std::string{ buffer.data() };
        command.pop_back(); // remove trailing \n

        auto parsed = parse_command(command);

        std::cout << "[Client " << clnt_no << "] command: " << parsed.first << ", params: " << parsed.second << std::endl;

        auto action = get_command_reply(parsed.first);
        action(clnt, parsed.second);
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

        auto auth = gap::server::persistant_authenticator{};

        auto srv = gap::server::server{ io_service, portno, auth};

        std::cout << "[Server] Gap is started, listening on port " << portno << "." << std::endl;

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
