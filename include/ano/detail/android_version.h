#pragma once

#include <stdexcept>
#include <sys/system_properties.h>

#include "error_quit.h"

namespace ano::detail {

inline unsigned int getAndroidVersion() {

    static unsigned int cache = []{
        char release_version[PROP_VALUE_MAX];
        __system_property_get("ro.build.version.release", release_version);
        try {
            return atoi(release_version);
        } catch (...) {
            errorQuit<std::runtime_error>("获取系统安卓版本失败");
        }
        return 0;  // never
    }();
    return cache;
}

}
