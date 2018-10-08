#pragma once

#include "authenticator.h"

#include <memory>
#include <string>

#include <odb/database.hxx>

namespace gap {
namespace server {
    struct db_authenticator : public authenticator {
        db_authenticator(std::string path);

        bool authenticate(const credentials& creds) override;

    private:
        std::unique_ptr<odb::database> db_;
    };
}
}
