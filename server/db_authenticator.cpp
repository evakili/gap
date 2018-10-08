#include "db_authenticator.h"
#include "db/user.h"

#include <odb/database.hxx>
#include <odb/sqlite/database.hxx>

namespace gap {
namespace server {

    db_authenticator::db_authenticator(std::string path)
        : db_ { std::make_unique<odb::sqlite::database>(path) }
    {}

    bool db_authenticator::authenticate(const credentials& creds)
    {
        db::user u{ creds.first, creds.second };
        return u.authenticate(creds.second);
    }
}
}
