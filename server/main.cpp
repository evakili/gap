/*
* This sample is based on source code from this tutorial:
*   http://www.cs.rpi.edu/~moorthy/Courses/os98/Pgms/socket.html
*/

#include <iostream>
#include <thread>
#include <boost/asio.hpp>

#include "argh.h"
#include "server.h"
#include "persistent_authenticator.h"

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
