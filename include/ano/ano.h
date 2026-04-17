#pragma once

#include <functional>
#include <memory>

#include <android/native_window.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <stdexcept>

#include "ano/detail/log.h"
#include "ano/detail/error_quit.h"
#include "ano/native_window_manager.h"

namespace ano {

struct Graphics {
    enum class API { OpenGLES3 };

    struct IContext {
        ANativeWindow*  nwindow;
        int             width;
        int             height;
        
        IContext(ANativeWindow* nwindow, int w, int h)
        : nwindow(nwindow), width(w), height(h) {
            ANativeWindow_acquire(nwindow);
        }

        ~IContext() {
            ANativeWindow_release(nwindow);
        }

        virtual void setCurrent() {}
        virtual void swapBuffers() {}
        
        int getWidth() const { return width; }
        int getHeight() const { return height; }
    };
    
    struct OpenGLContext : public IContext {
        struct EGL {
            EGLDisplay  display = EGL_NO_DISPLAY;
            EGLSurface  surface = EGL_NO_SURFACE;
            EGLContext  context = EGL_NO_CONTEXT;
        } egl;
        
        OpenGLContext(ANativeWindow* nwindow, int w, int h)
            : IContext(nwindow, w, h), egl(initEGL(nwindow, w, h)) {}
        
        ~OpenGLContext() {
            shutdownEGL(egl);
        }
        
        virtual void setCurrent() override {
            if (eglMakeCurrent(egl.display, egl.surface, egl.surface, egl.context) == EGL_FALSE) {
                detail::errorQuit<std::runtime_error>("设置选中失败: {:#x}", eglGetError());
            }
        }
        virtual void swapBuffers() override {
            if (eglSwapBuffers(egl.display, egl.surface) == EGL_FALSE) {
                detail::Log::error("交换缓冲区失败: {:#x}", eglGetError());
            }
        }
        
    private:
        static EGL initEGL(ANativeWindow* nwindow, int w, int h) {
            const EGLint attributes[] = {
                EGL_BLUE_SIZE,          8, 
                EGL_GREEN_SIZE,         8, 
                EGL_RED_SIZE,           8,
                EGL_ALPHA_SIZE,         8, 
                EGL_DEPTH_SIZE,         16,
                EGL_RENDERABLE_TYPE,    EGL_OPENGL_ES3_BIT, 
                EGL_SURFACE_TYPE,       EGL_WINDOW_BIT, 
                EGL_NONE
            };

            auto display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
            eglInitialize(display, nullptr, nullptr);
            EGLint num_configs = 0;
            eglChooseConfig(display, attributes, nullptr, 0, &num_configs);
            EGLConfig config;
            eglChooseConfig(display, attributes, &config, 1, &num_configs);
            EGLint format;
            eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);
            ANativeWindow_setBuffersGeometry(nwindow, 0, 0, format);

            const EGLint ctx_attributes[] = {EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE};
            auto context = eglCreateContext(display, config, EGL_NO_CONTEXT, ctx_attributes);
            auto surface = eglCreateWindowSurface(display, config, nwindow, nullptr);
            
            return {display, surface, context};
        }
        
        static void shutdownEGL(EGL& egl) {
            eglMakeCurrent(egl.display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
            eglDestroyContext(egl.display, egl.context);
            eglDestroySurface(egl.display, egl.surface);
            eglTerminate(egl.display);
            egl = {};
        }
    };
};

struct Window {
    using FramebufferSizeCallback = std::function<void(Window&, int, int)>;

    Graphics::API                       graphics_api;
    std::unique_ptr<Graphics::IContext> graphics_context;
    bool                                should_close;
    FramebufferSizeCallback             frame_buffer_size_callback;
};

inline std::unique_ptr<Window> createWindow(const char* title, int width, int height, Graphics::API graphics_api = Graphics::API::OpenGLES3) {
    auto w = NativeWindowManager::create(title, width, height);
    if (!w.has_value()) {
        detail::errorQuit<std::runtime_error>("创建窗口失败: {}", (int)w.error());
    }

    auto window = [=] -> std::unique_ptr<Graphics::IContext> {
        switch (graphics_api) {
        case Graphics::API::OpenGLES3:
            return std::make_unique<Graphics::OpenGLContext>(w.value(), width, height);
        default:
            detail::errorQuit<std::invalid_argument>("未支持的图形API: {}", (int)graphics_api);
        }
    }();

    return std::make_unique<Window>(graphics_api, std::move(window), false, nullptr);
}

inline void makeContextCurrent(const std::unique_ptr<Window>& window) {
    window->graphics_context->setCurrent();
}

inline void swapBuffers(const std::unique_ptr<Window>& window) {
    window->graphics_context->swapBuffers();
}

inline bool windowShouldClose(const std::unique_ptr<Window>& window) {
    return window->should_close;
}
inline void setWindowShouldClose(const std::unique_ptr<Window>& window, bool should_close) {
    window->should_close = should_close;
}

inline void pollEvents() {
    
}

inline void setFramebufferSizeCallback(const std::unique_ptr<Window>& window, const Window::FramebufferSizeCallback& fn) {
    if (!fn) {
        detail::errorQuit<std::invalid_argument>("设置帧缓冲区大小回调失败，fn is null");
    }
    window->frame_buffer_size_callback = fn;
    window->frame_buffer_size_callback(
        *window, 
        window->graphics_context->getWidth(), 
        window->graphics_context->getHeight()
    );
}

inline void destroyWindow(std::unique_ptr<Window> window) {
    auto nwindow = window->graphics_context->nwindow;
    window.reset(nullptr);
    
    if (!ano::NativeWindowManager::destroy(nwindow)) {
        detail::errorQuit<std::runtime_error>("销毁NativeWindow失败");
    }
}

}
