#pragma once

#include <cstdint>
#include <expected>
#include <optional>
#include <string>
#include <unordered_map>

#include <android/native_window.h>

#include "ano/common.h"
#include "ano/surface_control.h"
#include "ano/surface_composer_client.h"

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
    
        auto surface_control = g_window_control_map
            .emplace(name, SurfaceComposerClient::getInstance().createSurface(name.c_str(), width, height, {}, skip_screenshot))
            .first
            ->second;
        return g_window_map
            .emplace(name, (ANativeWindow*)surface_control.getSurface())
            .first
            ->second;
    }

    static std::optional<ANativeWindow*> find(const std::string& name) {
        if (auto it = g_window_map.find(name); it != g_window_map.end()) {
            return it->second;
        }
    
        return std::nullopt;
    }
    
    static bool destroy(const std::string& name) {
        auto window = find(name);
        if (window.value_or(nullptr) == nullptr) {
            return false;
        }
    
        g_window_control_map[name].destroySurface((Surface*)window.value());
    
        g_window_control_map.erase(name);
        g_window_map.erase(name);
    
        return true;
    }
    
    static  bool destroy(ANativeWindow* window) {
        auto name = [window] -> std::string {
            for (auto& pair : g_window_map) {
                if (pair.second == window) {
                    return std::string{pair.first};
                }
            }
            return {};
        }();
        if (name.empty()) {
            return false;
        }
        
        return destroy(name);
    }
private:
    static inline std::unordered_map<std::string, SurfaceControl> g_window_control_map;
    static inline std::unordered_map<std::string_view, ANativeWindow*> g_window_map;
};

}
