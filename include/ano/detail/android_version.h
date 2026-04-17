#pragma once

#include <sys/system_properties.h>

namespace ano::detail {

inline unsigned int getAndroidVersion() {
    char release_version[PROP_VALUE_MAX];
    __system_property_get("ro.build.version.release", release_version);
    try {
        return atoi(release_version);
    } catch (...) {}
    return 0;
}

}