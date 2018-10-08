#include "actions.h"

namespace gap {
namespace server {

    void default_action(client& clnt, std::string params, std::string message)
    {
        clnt.write(message);
    }

    void shutdown_action(client& clnt, std::string params)
    {
        if (clnt.is_authenticated()) {
            default_action(clnt, "", "Have a nice day...\n");
            std::exit(0);
        } else {
            clnt.write("You are not authenticated yet.\n");
        }
    }

    void bye_action(client& clnt, std::string params)
    {
        default_action(clnt, "", "See you soon...\n");
    }

    std::pair<std::string, std::string> parse_login_params(std::string params)
    {
        auto pos = params.find(';');
        auto username = params;
        auto password = std::string{};
        if (pos != std::string::npos) {
            username = params.substr(0, pos);
            password = params.substr(pos + 1);
        }
        return { username, password };
    }

    void login_action(client& clnt, std::string params)
    {
        if (params.empty()) {
            default_action(clnt, "", "Please provide username!\n");
        } else {
            if (clnt.try_login(parse_login_params(params)))
                default_action(clnt, "", "Hello " + clnt.username() + "!\n");
            else
                default_action(clnt, "", "Invalid username or password!\n");
        }
    }

    void logoff_action(client& clnt, std::string params)
    {
        clnt.logoff();
        default_action(clnt, "", "Miss you...\n");
    }

    void time_action(client& clnt, std::string params)
    {
        if (clnt.is_authenticated()) {
            auto now = std::time(nullptr);
            default_action(clnt, "", std::string{ std::ctime(&now) });
        } else {
            clnt.write("You are not authenticated yet.\n");
        }
    }

    const std::map<std::string, command_action> action_map = {
        { "login", login_action },
        { "logoff", logoff_action },
        { "bye", bye_action },
        { "time", time_action },
        { "shutdown", shutdown_action },
    };

    command_action get_command_reply(std::string command)
    {
        using namespace std::placeholders;
        auto action = action_map.find(command);
        if (action != action_map.end())
            return action->second;
        return std::bind(default_action, _1, _2, std::string{ "Unknow command, but no problem.\n" });
    }

    std::pair<std::string, std::string> parse_command(std::string command)
    {
        auto paramsPos = command.find(':');
        auto params = std::string{};
        if (paramsPos != std::string::npos) {
            params = command.substr(paramsPos + 1);
            command.erase(paramsPos);
        }
        return std::make_pair(command, params);
    }
}
}
