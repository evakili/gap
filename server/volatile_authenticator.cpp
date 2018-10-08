#include "volatile_authenticator.h"

namespace gap {
namespace server {

    bool volatile_authenticator::authenticate(const credentials& creds)
    {
        auto user = users_.find(creds.first);
        if (user != users_.end())
            if (user->second == creds.second)
                return true;
        return false;
    }

    const std::map<std::string, std::string> volatile_authenticator::users_ = {
        { "hasan", "123" },
        { "reza", "abc" },
        { "ali", "qaz" }
    };
}
}
