#pragma once

#include "authenticator.h"

#include <fstream>
#include <map>
#include <string>

namespace gap {
namespace server {
    struct persistant_authenticator : public authenticator {
        persistant_authenticator();

        bool authenticate(const credentials& creds) override;

    private:
        std::map<std::string, std::string> users_;
    };
}
}
