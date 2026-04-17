#pragma once

#include "ano/detail/symbol.h"
#include "ano/detail/android_version.h"
#include "common.h"

namespace ano {

class SurfaceControl
{
public:
    SurfaceControl(void* v = nullptr, int w = 0, int h = 0, bool skip_screenshot = false)
        : m_data(v), width(w), height(h), skip_screenshot(skip_screenshot) {}

    operator bool() const {
        return m_data != nullptr;
    }
    operator detail::android::StrongPointer<void>&() {
        static detail::android::StrongPointer<void> result;
        result.pointer = m_data;
        return result;
    }

    Surface* getSurface() {
        if (m_data == nullptr) return nullptr;

        auto strong = ANO_SYMBOL(SurfaceControl, getSurface).call(m_data);
        return (Surface*)((size_t)strong.pointer + ALIGN_OFFSET);
    }
    detail::android::StrongPointer<void> getParentingLayer() {
        if (m_data == nullptr) return {};
        if (detail::getAndroidVersion() < 12) {
            return static_cast<detail::android::StrongPointer<void>>(*this);
        }

        return ANO_SYMBOL(SurfaceControl, getParentingLayer).call(m_data);
    }
    void disConnect() {
        if (m_data == nullptr) return;
        
        ANO_SYMBOL(SurfaceControl, disConnect).call<>(m_data);
    }
    void setLayer(int32_t z) {
        if (m_data == nullptr || detail::getAndroidVersion() > 8) {
                return;
        }

        ANO_SYMBOL(SurfaceControl, setLayer).call(m_data, z);
    }
    void destroySurface(Surface* surface) {
        if (m_data == nullptr || surface == nullptr) {
                return;
        }

        auto surface_this = (Surface*)((size_t)(surface) - ALIGN_OFFSET);
        RefBase::decStrong(surface_this, this);
        if (detail::getAndroidVersion() < 7) {
                ANO_SYMBOL(Surface, disConnect).call<6>(surface_this, -1);
        } else {
                this->disConnect();
        }

        RefBase::decStrong(m_data, this);
    }
    void* getData() {
        return m_data;
    }
private:
    void* m_data;
public:
    int32_t width;
    int32_t height;
    bool skip_screenshot;
};

}
