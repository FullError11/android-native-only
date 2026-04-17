#pragma once

#include <algorithm>
#include <cstdint>
#include <expected>
#include <optional>
#include <string>
#include <unordered_map>

#include <android/native_window.h>

#include "ano/common.h"
#include "ano/surface_control.h"
#include "ano/surface_composer_client.h"
#include "ano/detail/log.h"

namespace ano {

struct NativeWindowManager {
    enum class ErrorCode {
        WindowAlreadyExists,  // 窗口已存在(name重复)
        WindowSizeInvalid,    // 创建大小不合法
    };

    static std::expected<ANativeWindow*, ErrorCode>
    create(const std::string& name, std::uint32_t width, std::uint32_t height, bool skip_screenshot = false) {
        if (g_window_map.contains(name)) {
            return std::unexpected(ErrorCode::WindowAlreadyExists);
        }
        if (width == 0 || height == 0) {
            return std::unexpected(ErrorCode::WindowSizeInvalid);
        }

        auto ctl = SurfaceComposerClient::getInstance().createSurface(name.c_str(), width, height, {}, skip_screenshot);
        auto window = reinterpret_cast<ANativeWindow*>(ctl.getSurface());

        g_window_control_map.emplace(window, ctl);
        g_window_map.emplace(name, window);
        return window;
    }

    static std::optional<ANativeWindow*> find(const std::string& name) {
        if (auto it = g_window_map.find(name); it != g_window_map.end()) {
            return it->second;
        }
    
        return std::nullopt;
    }
    
    static bool destroy(const std::string& name) {
        auto findAndMove = []<class K, class R>(
        std::unordered_map<K, R>& map, const K& k) -> std::optional<R> {
            auto it = map.find(k);
            if (it == map.end()) {
                return std::nullopt;
            }
            auto value = std::move(it->second);
            map.erase(it);
            return std::move(value);
        };
        
        auto window = findAndMove(g_window_map, name).value_or(nullptr);
        if (window == nullptr) {
            detail::Log::error("destory时未找到窗口");
            return false;
        }
        auto control = findAndMove(g_window_control_map, window);
        if (!control.has_value()) {
            detail::Log::error("destory时未找到SurfaceControl");
            return false;
        }

        control->destroySurface((Surface*)window);
        return true;
    }
    
    static  bool destroy(ANativeWindow* window) {
        auto it = std::find_if(g_window_map.begin(), g_window_map.end(), [window](auto& pair) {
            return pair.second == window;
        });
        
        if (it == g_window_map.end()) {
            detail::Log::error("destory时未在表内找到窗口名");
            return false;
        }
        auto& name = it->first;
        detail::Log::debug("查找到窗口 {:#x} 对应名: {}", (uintptr_t)window, name);
        
        return destroy(name);
    }
private:
    static inline std::unordered_map<std::string, ANativeWindow*> g_window_map;
    static inline std::unordered_map<ANativeWindow*, SurfaceControl> g_window_control_map;
};

}
