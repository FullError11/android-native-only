#pragma once

#include <cstring>

#include "ano/detail/symbol.h"

namespace ano {

using detail::android::MetadataType;
using detail::android::StrongPointer;
using detail::android::WindowFlags;
using detail::android::ui::Rotation;
using detail::android::ui::DisplayState;
using detail::android::ui::DisplayType;
using detail::android::ui::PhysicalDisplayId;

static constexpr uintptr_t ALIGN_OFFSET = sizeof(std::max_align_t) / 2;

#define ANO_SYMBOL(NAMESPACE, SYMBOL) \
    detail::Symbol::getInstance().Fn_##NAMESPACE##_##SYMBOL

struct Surface {};

class LayerMetadata
{
public:
    LayerMetadata() {
        std::memset(m_data, 0, 1024);
        ANO_SYMBOL(LayerMetadata, constructor).call(m_data);
    }

    void setInt32(detail::android::MetadataType key, int32_t value) {
        ANO_SYMBOL(LayerMetadata, setInt32).call<>(m_data, key, value);
    }

    operator void*() {
        return m_data;
    }
private:
    char m_data[1024];
};

class RefBase
{
public:
    RefBase() = delete;
    ~RefBase() = delete;

    static void incStrong(void* data, void* id) {
        ANO_SYMBOL(RefBase, incStrong).call(data, id);
    }
    static void decStrong(void* data, void* id) {
        ANO_SYMBOL(RefBase, decStrong).call(data, id);
    }
};

class String8
{
public:
    String8(const char* const str) {
        std::memset(m_data, 0, 1024);
        ANO_SYMBOL(String8, constructor).call(m_data, str);
    }
    ~String8() {
        ANO_SYMBOL(String8, destructor).call(m_data);
    }

    operator void*() {
        return m_data;
    }
private:
    char m_data[1024];
};

}
