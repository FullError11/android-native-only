#pragma once

#include <format>

#include <android/log.h>

namespace ano::detail {

class Log {
private:
    static constexpr const char *const TAG = "ANO";

public:
    static inline void trace(const char *msg) {
#ifndef NDEBUG
        __android_log_print(ANDROID_LOG_VERBOSE, TAG, "%s", msg);
#endif
    }

    template <class... Args>
    static inline void trace(std::format_string<Args...> fmt, Args &&...args) {
#ifndef NDEBUG
        __android_log_print(ANDROID_LOG_VERBOSE, TAG, "%s", 
            std::format(fmt, std::forward<Args &&>(args)...).c_str());
#endif
    }

    static inline void debug(const char *msg) {
#ifndef NDEBUG
        __android_log_print(ANDROID_LOG_DEBUG, TAG, "%s", msg);
#endif
    }

    template <class... Args>
    static inline void debug(std::format_string<Args...> fmt, Args &&...args) {
#ifndef NDEBUG
        __android_log_print(ANDROID_LOG_DEBUG, TAG, "%s",
            std::format(fmt, std::forward<Args &&>(args)...).c_str());
#endif
    }

    static inline void info(const char *msg) {
#ifndef NDEBUG
        __android_log_print(ANDROID_LOG_INFO, TAG, "%s", msg);
#endif
    }

    template <class... Args>
    static inline void info(std::format_string<Args...> fmt, Args &&...args) {
#ifndef NDEBUG
        __android_log_print(ANDROID_LOG_INFO, TAG, "%s",
            std::format(fmt, std::forward<Args &&>(args)...).c_str());
#endif
    }

    static inline void warn(const char *msg) {
#ifndef NDEBUG
        __android_log_print(ANDROID_LOG_WARN, TAG, "%s", msg);
#endif
    }

    template <class... Args>
    static inline void warn(std::format_string<Args...> fmt, Args &&...args) {
#ifndef NDEBUG
        __android_log_print(ANDROID_LOG_WARN, TAG, "%s",
            std::format(fmt, std::forward<Args &&>(args)...).c_str());
#endif
    }

    static inline void error(const char *msg) {
#ifndef NDEBUG
        __android_log_print(ANDROID_LOG_ERROR, TAG, "%s", msg);
#endif
    }

    template <class... Args>
    static inline void error(std::format_string<Args...> fmt, Args &&...args) {
#ifndef NDEBUG
        __android_log_print(ANDROID_LOG_ERROR, TAG, "%s",
            std::format(fmt, std::forward<Args &&>(args)...).c_str());
#endif
    }

    static inline void fatal(const char *msg) {
#ifndef NDEBUG
        __android_log_print(ANDROID_LOG_FATAL, TAG, "%s", msg);
#endif
    }

    template <class... Args>
    static inline void fatal(std::format_string<Args...> fmt, Args &&...args) {
#ifndef NDEBUG
        __android_log_print(ANDROID_LOG_FATAL, TAG, "%s",
            std::format(fmt, std::forward<Args &&>(args)...).c_str());
#endif
    }
};

} // namespace ano::detail
