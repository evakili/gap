#pragma once

#include "client.h"

#include <functional>
#include <string>
#include <map>

namespace gap {
namespace server {
    using command_action = std::function<void(client&, std::string)>;

    command_action get_command_reply(std::string command);
    std::pair<std::string, std::string> parse_command(std::string command);
}
}
