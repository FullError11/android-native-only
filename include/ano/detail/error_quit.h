#pragma once

#include <string>
#include <format>

#include "ano/detail/log.h"

namespace ano::detail {

template<class ThrowT>
void errorQuit(const char* msg) {
    Log::fatal("{}", msg);
    throw ThrowT(msg);
}

template<class ThrowT, class... Args>
void errorQuit(std::format_string<Args...> fmt, Args&&... args) {
    std::string msg = std::format(fmt, std::forward<Args&&>(args)...);
    Log::fatal("{}", msg);
    throw ThrowT(msg);
}

}