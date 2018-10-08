#pragma once

#include "authenticator.h"
#include "client.h"

#include <boost/asio.hpp>
using boost::asio::ip::tcp;

namespace gap {
namespace server {

    struct server {
        server(boost::asio::io_service& io_service, unsigned short portno, authenticator& auth)
            : io_service_{ io_service }
            , acceptor_{ io_service, tcp::endpoint{ tcp::v4(), portno } }
            , auth_{ auth }
        {
        }

        client next_client()
        {
            tcp::socket sock(io_service_);
            acceptor_.accept(sock);
            return client{ std::move(sock), auth_ };
        }

    private:
        boost::asio::io_service& io_service_;
        tcp::acceptor acceptor_;
        authenticator& auth_;
    };

    void gap_with_client(client clnt, int clnt_no);
}
}
