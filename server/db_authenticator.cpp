#include "db_authenticator.h"
#include "db/user.h"

namespace gap {
namespace server {

    bool db_authenticator::authenticate(const credentials& creds)
    {
        db::user u{ creds.first, creds.second };
        return u.authenticate(creds.second);
    }
}
}
