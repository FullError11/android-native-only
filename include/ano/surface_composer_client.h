#pragma once

#include <optional>

#include "ano/detail/android_version.h"
#include "ano/detail/symbol.h"
#include "surface_control.h"

namespace ano {

struct SurfaceComposerClient
{
public:
    struct Transaction {
    public:
        Transaction() {
            ANO_SYMBOL(SurfaceComposerClient_Transaction, constructor).call<>(m_data);
        }
        Transaction(const Transaction& other) {
            ANO_SYMBOL(SurfaceComposerClient_Transaction, copyConstructor).call<>((void*)m_data, (void*)other.m_data);
        }
    
        Transaction(Transaction&&) = delete;
    
        void* setLayer(detail::android::StrongPointer<void>& surface_control, int32_t z) {
            return ANO_SYMBOL(SurfaceComposerClient_Transaction, setLayer).call<>(m_data, surface_control, z);
        }
        void* setLayerStack(detail::android::StrongPointer<void>& surface_control, uint32_t layer_stack) {
            return ANO_SYMBOL(SurfaceComposerClient_Transaction, setLayerStack).call<>(m_data,surface_control,layer_stack);
        }
        void* setTrustedOverlay(detail::android::StrongPointer<void>& sirface_control, bool is_trusted_overlay) {
            return ANO_SYMBOL(SurfaceComposerClient_Transaction, setTrustedOverlay).call<>(m_data, sirface_control, is_trusted_overlay);
        }
    
        void show(detail::android::StrongPointer<void>& surface_control) {
            ANO_SYMBOL(SurfaceComposerClient_Transaction, show).call<>(m_data, surface_control);
        }
        void hide(detail::android::StrongPointer<void>& surface_control) {
            ANO_SYMBOL(SurfaceComposerClient_Transaction, hide).call<>(m_data, surface_control);
        }
        void reparent(detail::android::StrongPointer<void>& surface_control, detail::android::StrongPointer<void>& new_parent_handle)
        {
            ANO_SYMBOL(SurfaceComposerClient_Transaction, reparent).call<>(m_data, surface_control, new_parent_handle);
        }
        void setMatrix(detail::android::StrongPointer<void>& surface_control, float dsdx, float dtdx, float dsdy, float dtdy)
        {
            ANO_SYMBOL(SurfaceComposerClient_Transaction, setMatrix).call<>(m_data, surface_control, dsdx, dtdx, dsdy, dtdy);
        }

        void setPosition(detail::android::StrongPointer<void>& surface_control, float x, float y)
        {
            ANO_SYMBOL(SurfaceComposerClient_Transaction, setPosition).call<>(m_data, surface_control, x, y);
        }

        void setInputWindowInfo(detail::android::StrongPointer<void>& surface_control, void* window_info)
        {
            ANO_SYMBOL(SurfaceComposerClient_Transaction, setInputWindowInfo).call<>(m_data, surface_control, window_info);
        }

        int32_t apply(bool synchronous, bool one_way)
        {
            if (detail::getAndroidVersion() <= 12) {
                return ANO_SYMBOL(SurfaceComposerClient_Transaction, apply).call<12>(m_data, synchronous);
            } else {
                return ANO_SYMBOL(SurfaceComposerClient_Transaction, apply).call<>(m_data, synchronous, one_way);
            }
        }

    private:
        char m_data[1024];
    };

    SurfaceComposerClient() {
        ANO_SYMBOL(SurfaceComposerClient, constructor).call<>(m_data);
        RefBase::incStrong(m_data, this);
    }

    SurfaceControl createSurface(const char* n, int w, int h, detail::android::WindowFlags flags = {}, bool skip_screenshot = false) {
        static void* parent = nullptr;
        parent = nullptr;

        String8 name(n);
        detail::android::PixelFormat pix_fmt(detail::android::PixelFormat::RGBA_8888);
        LayerMetadata layer {};
        detail::android::StrongPointer<void> result {};

        auto android_version = detail::getAndroidVersion();
        auto func = ANO_SYMBOL(SurfaceComposerClient, createSurface);

        switch (android_version) {
            case 5: case 6: case 7: {
                func.call<7>(m_data, name, w, h, pix_fmt, flags);
                break;
            }
            case 8: case 9: {
                uint32_t type = skip_screenshot ? detail::android::WINDOW_TYPE_DONT_SCREENSHOT : 0;
                func.call<9>(m_data, name, w, h, pix_fmt, flags, parent, type, 0);
                break;
            }
            case 10: case 11: {
                if (skip_screenshot)
                    layer.setInt32(detail::android::MetadataType::WINDOW_TYPE, detail::android::WINDOW_TYPE_DONT_SCREENSHOT);
                if (android_version == 10) {
                    result = func.call<10>(m_data, name, w, h, pix_fmt, flags, parent, layer);
                } else {
                    result = func.call<11>(m_data, name, w, h, pix_fmt, flags, parent, layer, nullptr);
                }
                break;
            }
            default: {
                if (skip_screenshot)
                    flags |= detail::android::WindowFlags::eSkipScreenshot;
                result = func.call<-1>(m_data, name, w, h, pix_fmt, flags, &parent, layer, nullptr);
            }
        };

        if (android_version >= 12) {
            static Transaction transaction;
            transaction.setTrustedOverlay(result, true);
            transaction.setLayer(result, INT32_MAX);
            transaction.apply(false, true);
        } else if (android_version <= 8) {
            openGlobalTransaction();
            SurfaceControl surface(result.get());
            surface.setLayer(INT32_MAX);
            closeGlobalTransaction(false);
        }

        return SurfaceControl(result.get(), w, h, skip_screenshot);
    }
    SurfaceControl mirrorSurface(SurfaceControl& surface, uint32_t layer_stack) {
        using MirrorSurfaces = std::pair<void* , void* >;
        constexpr auto mirrorSurfacesDeleter = [](MirrorSurfaces* pair) {
            SurfaceControl fake_surface;

            // pair的两个值都是SurfaceControl，
            // 但因为他们的结构是逆向解析来的，只是一个足够打的数据块，
            // 只能这样调用
            auto disConnect = ANO_SYMBOL(SurfaceControl, disConnect);

            disConnect.call(pair->first);
            RefBase::decStrong(pair->first, &fake_surface);
                
            disConnect.call(pair->second);
            RefBase::decStrong(pair->second, &fake_surface);

            delete pair;
        };

        using MirrorSurfacesProxy = std::unique_ptr<MirrorSurfaces, decltype(mirrorSurfacesDeleter)>;

        if (auto version = detail::getAndroidVersion(); version < 14) {
                detail::Log::debug("skip: system version {} < 14, need not it", static_cast<size_t>(version));
                return {};
        }

        if (surface.skip_screenshot) {
                detail::Log::warn("skip: surface is skip screenshot rrecording");
                return {};
        }

        if (surface.width <= 0 || surface.height <= 0) {
                detail::Log::warn("skip: surface size={}x{}", surface.width, surface.height);
                return {};
        }

        auto mirror_surface = ANO_SYMBOL(SurfaceComposerClient, mirrorSurface).call(m_data, surface.getData());

        auto mirror_root_name = "MirrorRoot@" + std::to_string(layer_stack);
        auto mirror_root_surface = createSurface(mirror_root_name.c_str(), surface.width, surface.height, detail::android::WindowFlags::eNoColorFill);
        if (!mirror_root_surface) {
                detail::Log::error("failed to create root surface");
                return {};
        }

        static Transaction transaction;
        static std::vector<MirrorSurfacesProxy> mirror_surfaces;

        transaction.setLayer(mirror_root_surface, INT_MAX);
        transaction.setLayerStack(mirror_root_surface, layer_stack);
        transaction.apply(false, true);

        transaction.setLayerStack(mirror_surface, layer_stack);
        transaction.show(mirror_surface);
        transaction.reparent(mirror_surface, mirror_root_surface);
        transaction.apply(false, true);

        mirror_surfaces.emplace_back(new MirrorSurfaces{mirror_surface.get(), mirror_root_surface.getData()}, mirrorSurfacesDeleter);

        return mirror_root_surface;
    }

    void zoomSurface(SurfaceControl& surface, float scale_x, float scale_y) {
        static Transaction transaction;

        transaction.setMatrix(surface, scale_x, 0.f, 0.f, scale_y);
        transaction.apply(false, true);
    }
    void moveSurface(SurfaceControl& surface, float x, float y) {
        static Transaction transaction;

        transaction.setPosition(surface, x, y);
        transaction.apply(false, true);
    }
    std::optional<detail::android::ui::DisplayState> getDisplayInfo() {
        detail::android::ui::DisplayState result;

        detail::android::StrongPointer<void> display;
        auto android_version = detail::getAndroidVersion();
        if (android_version <= 9) {
            display = getBuiltInDisplay(detail::android::ui::DisplayType::DisplayIdMain);
        } else if (android_version >= 10 && android_version <= 13) {
                display = getInternalDisplayToken();
        } else {
                auto display_ids = getPhysicalDisplayIds();
                if (display_ids.empty()) {
                        detail::Log::error("display id array is empty");
                        return std::nullopt;
                }

                display = getPhysicalDisplayToken(display_ids[0]);
        }

        if (display.get() == nullptr) {
            detail::Log::error("failed to get display");
            return std::nullopt;
        }

        if (android_version >= 11) {
            if (detail::android::ui::DisplayState buffer {}; getDisplayState(display, &buffer) == 0) {
                return buffer;
            }
            return std::nullopt;
        } else {
            detail::android::ui::DisplayInfo info;
            if (getDisplayInfo(display, &info) == 0) {
                return detail::android::ui::DisplayState {
                    .orientation = static_cast<detail::android::ui::Rotation>(info.orientation),
                    .layerStackSpaceRect = { static_cast<int32_t>(info.w), static_cast<int32_t>(info.h) }
                };
            }
        }

        detail::Log::error("failed to get display info");
        return std::nullopt;
    }
    Transaction& getDefaultTransaction() {
        static Transaction transaction;
        return transaction;
    }

    static SurfaceComposerClient& getInstance() {
        static SurfaceComposerClient instance;
        return instance;
    }

    static detail::android::StrongPointer<void> getInternalDisplayToken() {
        return ANO_SYMBOL(SurfaceComposerClient, getInternalDisplayToken).call<>();
    }
    static detail::android::StrongPointer<void> getBuiltInDisplay(detail::android::ui::DisplayType t) {
        return ANO_SYMBOL(SurfaceComposerClient, getBuiltInDisplay).call<>(t);
    }
    static int32_t getDisplayState(detail::android::StrongPointer<void>& sp, detail::android::ui::DisplayState* s) {
        return ANO_SYMBOL(SurfaceComposerClient, getDisplayState).call<>(sp, s);
    }
    static int32_t getDisplayInfo(detail::android::StrongPointer<void>& sp, detail::android::ui::DisplayInfo* i) {
        return ANO_SYMBOL(SurfaceComposerClient, getDisplayInfo).call<>(sp, i);
    }
    static std::vector<detail::android::ui::PhysicalDisplayId> getPhysicalDisplayIds() {
        return ANO_SYMBOL(SurfaceComposerClient, getPhysicalDisplayIds).call();
    }
    static detail::android::StrongPointer<void> getPhysicalDisplayToken(detail::android::ui::PhysicalDisplayId id) {
        return ANO_SYMBOL(SurfaceComposerClient, getPhysicalDisplayToken).call(id);
    }
    static void openGlobalTransaction() {
        ANO_SYMBOL(SurfaceComposerClient, openGlobalTransaction).call<>();
    }
    static void closeGlobalTransaction(bool sync = false) {
        ANO_SYMBOL(SurfaceComposerClient, closeGlobalTransaction).call<>(sync);
    }
private:
    char m_data[1024];
};

}
