#pragma once

#include "authenticator.h"

namespace gap {
namespace server {
    struct db_authenticator : public authenticator {
        bool authenticate(const credentials& creds) override;
    };
}
}
