#pragma once

#include <system_error>

inline void error() {
    throw std::system_error{ errno, std::system_category().default_error_condition(errno).category() };
}
