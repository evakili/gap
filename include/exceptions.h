#pragma once

#include <system_error>

namespace gap {

struct posix_error : std::system_error {
    posix_error() :
        std::system_error{ errno, std::system_category().default_error_condition(errno).category() }
    {}
};

}
