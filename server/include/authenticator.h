#pragma once

#include <utility>
#include <string>

namespace gap {
namespace server {

    using credentials = std::pair<std::string, std::string>;

    struct authenticator {
        virtual ~authenticator() = default;

        virtual bool authenticate(const credentials& creds) = 0;
    };

}
}