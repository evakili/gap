#pragma once

#include "authenticator.h"

#include <boost/asio.hpp>
using boost::asio::ip::tcp;

namespace gap {
namespace server {

    struct client {
        explicit client(tcp::socket sock, authenticator& auth)
            : sock_{ std::move(sock) }
            , authenticated_{ false }
            , username_{}
            , auth_{ auth }
        {
        }

        template <typename Container>
        auto write(const Container& buffer)
        {
            return boost::asio::write(sock_, boost::asio::buffer(buffer));
        }

        template <typename Container>
        auto read(Container& buffer)
        {
            using namespace std::placeholders;
            boost::system::error_code ec;
            auto n = boost::asio::read(sock_, boost::asio::buffer(buffer),
                std::bind(&client::read_complete, this, buffer.data(), _1, _2), ec);
            if (!ec || ec == boost::asio::error::eof)
                return n; // Connection closed cleanly by peer.
            else
                throw boost::system::system_error(ec);
        }

        bool is_authenticated()
        {
            return authenticated_;
        }

        std::string username()
        {
            return username_;
        }

        bool try_login(const credentials& creds)
        {
            authenticated_ = false;
            if (auth_.authenticate(creds)) {
                username_ = creds.first;
                authenticated_ = true;
            }
            return authenticated_;
        }

        void logoff()
        {
            username_.clear();
            authenticated_ = false;
        }

    private:
        size_t read_complete(const char* buf, const boost::system::error_code& err, size_t bytes)
        {
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
}
}
