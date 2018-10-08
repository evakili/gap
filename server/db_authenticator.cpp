#include "db_authenticator.h"
#include "db/user.h"
#include "db/user_odb.h"

#include <odb/database.hxx>
#include <odb/sqlite/database.hxx>

namespace gap {
namespace server {

    db_authenticator::db_authenticator(std::string path)
        : db_ { std::make_unique<odb::sqlite::database>(path) }
    {}

    bool db_authenticator::authenticate(const credentials& creds)
    {
        try {
            odb::transaction t{ db_->begin() };

            auto u = std::unique_ptr<db::user>{ db_->load<db::user>(creds.first) };

            t.commit();

            return u->authenticate(creds.second);
        }
        catch(odb::object_not_persistent&) {
            return false;
        }
    }
}
}
