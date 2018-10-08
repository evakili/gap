#include "server.h"
#include "actions.h"

#include <iostream>

namespace gap {
namespace server {

    void gap_with_client(client clnt, int clnt_no)
    {
        while (true) {
            std::cout << "[Server] Client " << clnt_no << " is connected." << std::endl;
            auto buffer = std::array<char, 256>{};
            if (clnt.read(buffer) <= 0) {
                std::cout << "[Server] Client " << clnt_no << " is down.\n"
                          << std::endl;
                return;
            }

            auto command = std::string{ buffer.data() };
            command.pop_back(); // remove trailing \n

            auto parsed = parse_command(command);

            std::cout << "[Client " << clnt_no << "] command: " << parsed.first << ", params: " << parsed.second << std::endl;

            auto action = get_command_reply(parsed.first);
            action(clnt, parsed.second);
        };
    }
}
}