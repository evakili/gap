#pragma once

#include "authenticator.h"

#include <map>
#include <string>

namespace gap {
namespace server {

    struct volatile_authenticator : public authenticator {
        bool authenticate(const credentials& creds) override;

    private:
        static const std::map<std::string, std::string> users_;
    };
}
}
