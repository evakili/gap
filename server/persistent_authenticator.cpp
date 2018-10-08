#include "persistent_authenticator.h"

namespace gap {
namespace server {

    persistant_authenticator::persistant_authenticator()
    {
        auto in = std::ifstream{ "/etc/gap/users.dat" };
        auto username = std::string{};
        auto password = std::string{};
        while (in >> username >> password)
            users_.insert(std::make_pair(username, password));
    }

    bool persistant_authenticator::authenticate(const credentials& creds)
    {
        auto user = users_.find(creds.first);
        if (user != users_.end())
            if (user->second == creds.second)
                return true;
        return false;
    }
}
}
