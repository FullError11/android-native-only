#pragma once

#include <cstdint>
#include <string>

#include <cstddef>
#include <dlfcn.h>

#include "ano/detail/log.h"
#include "ano/detail/android_version.h"
#include "ano/detail/util/range_map.h"

namespace ano::detail {

static constexpr int8_t MIN_SUPPORT_ANDROID_VERSION = 5;
static constexpr int8_t MAX_SUPPORT_ANDROID_VERSION = 16;

namespace android {
    enum class PixelFormat {
        UNKNOWN      = 0,
        CUSTOM       = -4,
        TRANSLUCENT  = -3,
        TRANSPARENT  = -2,
        OPAQUE       = -1,
        RGBA_8888    = 1,
        RGBX_8888    = 2,
        RGB_888      = 3,
        RGB_565      = 4,
        BGRA_8888    = 5,
        RGBA_5551    = 6,
        RGBA_4444    = 7,
        RGBA_FP16    = 22,
        RGBA_1010102 = 43,
        R_8          = 0x38,
    };

    enum class WindowFlags : uint32_t {
        eHidden               = 0x00000004,
        eDestroyBackbuffer    = 0x00000020,
        eSkipScreenshot       = 0x00000040,
        eSecure               = 0x00000080,
        eNonPremultiplied     = 0x00000100,
        eOpaque               = 0x00000400,
        eProtectedByApp       = 0x00000800,
        eProtectedByDRM       = 0x00001000,
        eCursorWindow         = 0x00002000,
        eNoColorFill          = 0x00004000,

        eFXSurfaceBufferQueue = 0x00000000,
        eFXSurfaceEffect      = 0x00020000,
        eFXSurfaceBufferState = 0x00040000,
        eFXSurfaceContainer   = 0x00080000,
        eFXSurfaceMask        = 0x000F0000,
    };

    enum class MetadataType : uint32_t {
        OWNER_UID        = 1,
        WINDOW_TYPE      = 2,
        TASK_ID          = 3,
        MOUSE_CURSOR     = 4,
        ACCESSIBILITY_ID = 5,
        OWNER_PID        = 6,
        DEQUEUE_TIME     = 7,
        GAME_MODE        = 8
    };

    enum {
        WINDOW_TYPE_DONT_SCREENSHOT = 441731
    };


    template <typename T>
    struct StrongPointer {
        union {
            T* pointer;
            char padding[sizeof(std::max_align_t)];
        };

        T* operator->() const { return pointer; }
        T* get() const { return pointer; }

        explicit operator bool() const {
            return nullptr != pointer;
        }
    };

    template <typename T>
    constexpr T operator|(T lhs, T rhs) {
        static_assert(std::is_enum_v<T>, "T must is enum");
        using underlying_t = std::underlying_type_t<T>;
        return static_cast<T>(static_cast<underlying_t>(lhs) | static_cast<underlying_t>(rhs));
    }

    template <typename T>
    constexpr T operator|=(T& lhs, T rhs) {
        static_assert(std::is_enum_v<T>, "T must is enum");
        return (lhs = lhs | rhs);
    }

}  // !namespace android

namespace android::ui
{
    using nsecs_t = int64_t;

    enum class Rotation {
        Rotation0   = 0,
        Rotation90  = 1,
        Rotation180 = 2,
        Rotation270 = 3
    };

    enum class DisplayType {
        DisplayIdMain = 0,
        DisplayIdHdmi = 1
    };

    // 表示按 Z序 排列的图层组
    // 每个图层只能关联到一个 LayerStack
    // 一个 LayerStack 可以关联到多个显示器,以显示相同内容
    struct LayerStack {
        uint32_t id = UINT32_MAX;
    };

    struct Size {
        int32_t width  = -1;
        int32_t height = -1;
    };

        // 显示的事物状态
        // libgui将android::DisplayState定义为android::ui::DisplayState的超集
    struct DisplayState {
        LayerStack layerStack;
        Rotation orientation = Rotation::Rotation0;
        Size layerStackSpaceRect;
    };

    struct DisplayInfo {
        uint32_t w                   {0};
        uint32_t h                   {0};
        float xdpi                   {0};
        float ydpi                   {0};
        float fps                    {0};
        float density                {0};
        uint8_t orientation          {0};
        bool secure                  {false};
        nsecs_t appVsyncOffset       {0};
        nsecs_t presentationDeadline {0};
        uint32_t viewportW           {0};
        uint32_t viewportH           {0};
    };

    struct PhysicalDisplayId {
        uint64_t value;
    };
}  // !namespace android::ui

class Symbol {
public:
    using SVCode = std::int8_t;
    
    Symbol(const Symbol&) = delete;
    Symbol& operator=(const Symbol&) = delete;
    
    Symbol(Symbol&&) = delete;
    Symbol& operator=(Symbol&&) = delete;
private:
    using Map = RangeMap<SVCode, std::string>;

    static constexpr std::uint8_t SV_MIN = MIN_SUPPORT_ANDROID_VERSION;
    static constexpr std::uint8_t SV_MAX = MAX_SUPPORT_ANDROID_VERSION;

    struct String {
        // ----- libgui -----
        static inline Map LayerMetadata_constructor = {
            {10, 13, "_ZN7android13LayerMetadataC2Ev"},
            {14, SV_MAX, "_ZN7android3gui13LayerMetadataC2Ev"},
        };

        static inline Map LayerMetadata_setInt32{
            {10, 13, "_ZN7android13LayerMetadata8setInt32Eji"},
            {14, SV_MAX, "_ZN7android3gui13LayerMetadata8setInt32Eji"},
        };

        static inline Map SurfaceComposerClient_constructor{
            {SV_MIN, SV_MAX, "_ZN7android21SurfaceComposerClientC2Ev"},
        };
        static inline Map SurfaceComposerClient_mirrorSurface{
            {11, SV_MAX, "_ZN7android21SurfaceComposerClient13mirrorSurfaceEPNS_14SurfaceControlE"},
        };
        static inline Map SurfaceComposerClient_getInternalDisplayToken{
            {10, 13, "_ZN7android21SurfaceComposerClient23getInternalDisplayTokenEv"},
        };

        static inline Map SurfaceComposerClient_getBuiltInDisplay{
            {SV_MIN, 9, "_ZN7android21SurfaceComposerClient17getBuiltInDisplayEi"}, 
        };
        static inline Map SurfaceComposerClient_getDisplayState{
            {11, SV_MAX, "_ZN7android21SurfaceComposerClient15getDisplayStateERKNS_2spINS_7IBinderEEEPNS_2ui12DisplayStateE"}, 
        };
        static inline Map SurfaceComposerClient_getDisplayInfo{
            {SV_MIN, 11, "_ZN7android21SurfaceComposerClient14getDisplayInfoERKNS_2spINS_7IBinderEEEPNS_11DisplayInfoE"}, 
        };
        static inline Map SurfaceComposerClient_getPhysicalDisplayIds{
            {10, SV_MAX, "_ZN7android21SurfaceComposerClient21getPhysicalDisplayIdsEv"}, 
        };

        static inline Map SurfaceComposerClient_getPhysicalDisplayToken{
            {12, SV_MAX, "_ZN7android21SurfaceComposerClient23getPhysicalDisplayTokenENS_17PhysicalDisplayIdE"}, 
        };

        // SurfaceComposerClient_Transaction
        static inline Map SurfaceComposerClient_Transaction_copyConstructor{
            {11, 11, "_ZN7android21SurfaceComposerClient11TransactionC2ERKS1_"}, 
        };

        static inline Map SurfaceComposerClient_Transaction_constructor{
            {12, SV_MAX, "_ZN7android21SurfaceComposerClient11TransactionC2Ev"}, 
        };

        static inline Map SurfaceComposerClient_Transaction_setLayer{
            {9, SV_MAX, "_ZN7android21SurfaceComposerClient11Transaction8setLayerERKNS_2spINS_14SurfaceControlEEEi"}, 
        };

        static inline Map SurfaceComposerClient_Transaction_setTrustedOverlay{
            {12, SV_MAX, "_ZN7android21SurfaceComposerClient11Transaction17setTrustedOverlayERKNS_2spINS_14SurfaceControlEEEb"}, 
        };

        static inline Map SurfaceComposerClient_Transaction_setLayerStack{
            {13, SV_MAX, "_ZN7android21SurfaceComposerClient11Transaction13setLayerStackERKNS_2spINS_14SurfaceControlEEENS_2ui10LayerStackE"}, 
        };

        static inline Map SurfaceComposerClient_Transaction_show{
            {9, SV_MAX, "_ZN7android21SurfaceComposerClient11Transaction4showERKNS_2spINS_14SurfaceControlEEE"}, 
        };

        static inline Map SurfaceComposerClient_Transaction_hide{
            {9, SV_MAX, "_ZN7android21SurfaceComposerClient11Transaction4hideERKNS_2spINS_14SurfaceControlEEE"}, 
        };

        static inline Map SurfaceComposerClient_Transaction_reparent{
            {12, SV_MAX, "_ZN7android21SurfaceComposerClient11Transaction8reparentERKNS_2spINS_14SurfaceControlEEES6_"}, 
        };

        static inline Map SurfaceComposerClient_Transaction_setMatrix{
            {9, SV_MAX, "_ZN7android21SurfaceComposerClient11Transaction9setMatrixERKNS_2spINS_14SurfaceControlEEEffff"}, 
        };

        static inline Map SurfaceComposerClient_Transaction_setPosition{
            {9, SV_MAX, "_ZN7android21SurfaceComposerClient11Transaction11setPositionERKNS_2spINS_14SurfaceControlEEEff"}, 
        };

        // SurfaceControl
        static inline Map SurfaceControl_getSurface{
            {SV_MIN, 11, "_ZNK7android14SurfaceControl10getSurfaceEv"},
            {12, SV_MAX, "_ZN7android14SurfaceControl10getSurfaceEv"},
        };
        
        static inline Map SurfaceControl_disConnect{
            {SV_MIN, 6, "_ZN7android7Surface10disconnectEi"},
            {7, SV_MAX, "_ZN7android14SurfaceControl10disconnectEv"},
        };
        static inline Map SurfaceControl_getParentingLayer{
            {12, SV_MAX, "_ZN7android14SurfaceControl17getParentingLayerEv"},
        };

        static inline Map SurfaceComposerClient_createSurface{
            {SV_MIN, 7, "_ZN7android21SurfaceComposerClient13createSurfaceERKNS_7String8Ejjij"}, 
            {8, 8, "_ZN7android21SurfaceComposerClient13createSurfaceERKNS_7String8EjjijPNS_14SurfaceControlEjj"}, 
            {9, 9, "_ZN7android21SurfaceComposerClient13createSurfaceERKNS_7String8EjjijPNS_14SurfaceControlEii"},
            {10, 10, "_ZN7android21SurfaceComposerClient13createSurfaceERKNS_7String8EjjijPNS_14SurfaceControlENS_13LayerMetadataE"}, 
            {11, 11, "_ZN7android21SurfaceComposerClient13createSurfaceERKNS_7String8EjjijPNS_14SurfaceControlENS_13LayerMetadataEPj"}, 
            {12, 13, "_ZN7android21SurfaceComposerClient13createSurfaceERKNS_7String8EjjijRKNS_2spINS_7IBinderEEENS_13LayerMetadataEPj"}, 
            {14, SV_MAX, "_ZN7android21SurfaceComposerClient13createSurfaceERKNS_7String8EjjiiRKNS_2spINS_7IBinderEEENS_3gui13LayerMetadataEPj"},
        };
        static inline Map SurfaceComposerClient_openGlobalTransaction{
            {SV_MIN, 7, "_ZN7android21SurfaceComposerClient21openGlobalTransactionEv"}, 
        };
        static inline Map SurfaceComposerClient_closeGlobalTransaction{
            {SV_MIN, 7, "_ZN7android21SurfaceComposerClient22closeGlobalTransactionEb"}, 
        };
        static inline Map SurfaceControl_setLayer{
            {SV_MIN, SV_MIN, "_ZN7android14SurfaceControl8setLayerEi"},
            {6, 7, "_ZN7android14SurfaceControl8setLayerEj"}, 
        };
        static inline Map Surface_disConnect{
            {5, 6, "_ZN7android7Surface10disconnectEi"}, 
            {7, 7, "_ZN7android14SurfaceControl10disconnectEv"}, 
        };

        static inline Map SurfaceComposerClient_Transaction_apply{
            {8, 12, "_ZN7android21SurfaceComposerClient11Transaction5applyEb"}, 
            {13, SV_MAX, "_ZN7android21SurfaceComposerClient11Transaction5applyEbb"}, 
        };
        static inline Map SurfaceComposerClient_Transaction_setInputWindowInfo{
            {10, 12, "_ZN7android21SurfaceComposerClient11Transaction18setInputWindowInfoERKNS_2spINS_14SurfaceControlEEERKNS_15InputWindowInfoE"}, 
            {13, 15, "_ZN7android21SurfaceComposerClient11Transaction18setInputWindowInfoERKNS_2spINS_14SurfaceControlEEERKNS_3gui10WindowInfoE"}, 
            {16, SV_MAX, "_ZN7android21SurfaceComposerClient11Transaction18setInputWindowInfoERKNS_2spINS_14SurfaceControlEEENS2_INS_3gui16WindowInfoHandleEEE"}, 
        };


        // ----- libutils -----
        static inline Map RefBase_incStrong{
            {SV_MIN, SV_MAX, "_ZNK7android7RefBase9incStrongEPKv"}, 
        };
        static inline Map RefBase_decStrong{
            {SV_MIN, SV_MAX, "_ZNK7android7RefBase9decStrongEPKv"}, 
        };

        static inline Map String8_constructor{
            {SV_MIN, SV_MAX, "_ZN7android7String8C2EPKc"}, 
        };
        static inline Map String8_destructor{
            {SV_MIN, SV_MAX, "_ZN7android7String8D2Ev"}, 
        };
    };
    public:
#define ANO_DEFINE_SYMBOL(NAMESPACE, SYMBOL, DEFAULT_SIGN, ...) \
struct {                                                \
    template<Symbol::SVCode V = 0> struct Sign {        \
        using type = DEFAULT_SIGN;                      \
    };                                                  \
    __VA_ARGS__                                         \
    void* fn;                                           \
    void set(void* p) { fn = p; }                       \
    template<class T> struct FTraits;                   \
    template<class R, class... Args>                    \
    struct FTraits<R(*)(Args...)> { using Ret = R; };   \
    template<Symbol::SVCode V = -1, class... Args>      \
    auto call(Args&&... args) {                         \
        using Fn = typename Sign<V>::type;              \
        using Ret = typename FTraits<Fn>::Ret;          \
        return ((Fn)fn)(std::forward<Args>(args)...);   \
    }                                                   \
} Fn_##NAMESPACE##_##SYMBOL;

#define ANO_SYM_SIGN(V, SIGN) \
        template<> struct Sign<V> { using type = SIGN; }
#define ANO_USE_SYM_SIGN(FROM, TO) \
        template<> struct Sign<TO> { using type = Sign<FROM>::type; };

        using VSP = android::StrongPointer<void>;
        using PixFmt = android::PixelFormat;
        using WFlag = android::WindowFlags;


ANO_DEFINE_SYMBOL(LayerMetadata, constructor,              void(*)(void*));
ANO_DEFINE_SYMBOL(LayerMetadata, setInt32,                 void(*)(void*, android::MetadataType key, int32_t value));
ANO_DEFINE_SYMBOL(SurfaceComposerClient, constructor,      void*(*)(void*));
ANO_DEFINE_SYMBOL(SurfaceComposerClient, mirrorSurface,    VSP(*)(void*, void* mirror_from_surface));

/*     static     */
ANO_DEFINE_SYMBOL(SurfaceComposerClient, getInternalDisplayToken,  VSP(*)());
ANO_DEFINE_SYMBOL(SurfaceComposerClient, getBuiltInDisplay,        VSP(*)(android::ui::DisplayType display));
ANO_DEFINE_SYMBOL(SurfaceComposerClient, getDisplayState,          int32_t(*)(VSP& display, android::ui::DisplayState* display_state));
ANO_DEFINE_SYMBOL(SurfaceComposerClient, getDisplayInfo,           int32_t(*)(VSP& display, android::ui::DisplayInfo* display_info));
ANO_DEFINE_SYMBOL(SurfaceComposerClient, getPhysicalDisplayIds,    std::vector<android::ui::PhysicalDisplayId>(*)());
ANO_DEFINE_SYMBOL(SurfaceComposerClient, getPhysicalDisplayToken,  VSP(*)(android::ui::PhysicalDisplayId display_id));

ANO_DEFINE_SYMBOL(SurfaceComposerClient_Transaction, copyConstructor,      void*(*)(void*, void* other));
ANO_DEFINE_SYMBOL(SurfaceComposerClient_Transaction, constructor,          void*(*)(void*));
ANO_DEFINE_SYMBOL(SurfaceComposerClient_Transaction, setLayer,             void*(*)(void*, VSP& surface_control, int32_t z));
ANO_DEFINE_SYMBOL(SurfaceComposerClient_Transaction, setTrustedOverlay,    void*(*)(void*, VSP& surface_control, bool is_trusted_overlay));
ANO_DEFINE_SYMBOL(SurfaceComposerClient_Transaction, setLayerStack,        void*(*)(void*, VSP& surface_control, uint32_t layer_stack));
ANO_DEFINE_SYMBOL(SurfaceComposerClient_Transaction, show,                 void*(*)(void*, VSP& surface_control));
ANO_DEFINE_SYMBOL(SurfaceComposerClient_Transaction, hide,                 void*(*)(void*, VSP& surface_control));
ANO_DEFINE_SYMBOL(SurfaceComposerClient_Transaction, reparent,             void*(*)(void*, VSP& surface_control, VSP& new_parent_handle));
ANO_DEFINE_SYMBOL(SurfaceComposerClient_Transaction, setMatrix,            void*(*)(void*, VSP& surface_control, float dsdx, float dtdx, float dtdy, float dsdy));
ANO_DEFINE_SYMBOL(SurfaceComposerClient_Transaction, setPosition,          void*(*)(void*, VSP& surface_control, float x, float y));

ANO_DEFINE_SYMBOL(SurfaceControl, getSurface,          VSP(*)(void*));
ANO_DEFINE_SYMBOL(SurfaceControl, disConnect,          void(*)(void*));
ANO_DEFINE_SYMBOL(SurfaceControl, getParentingLayer,   VSP(*)(void*));

ANO_DEFINE_SYMBOL(SurfaceComposerClient, createSurface, VSP(*)(void*, void* name, uint32_t w, uint32_t h, PixFmt fmt, WFlag flags),
    ANO_SYM_SIGN(8,  VSP(*)(void*, void* name, uint32_t w, uint32_t h, PixFmt fmt, WFlag flags, void* parent, uint32_t window_type, uint32_t owner_uid));
    ANO_USE_SYM_SIGN(8, 9);
    ANO_SYM_SIGN(10, VSP(*)(void*, void* name, uint32_t w, uint32_t h, PixFmt fmt, WFlag flags, void* parent, void* metadata));
    ANO_SYM_SIGN(11, VSP(*)(void*, void* name, uint32_t w, uint32_t h, PixFmt fmt, WFlag flags, void* parent, void* metadata, uint32_t* out_transform_hint));
    ANO_SYM_SIGN(12, VSP(*)(void*, void* name, uint32_t w, uint32_t h, PixFmt fmt, WFlag flags, void** parent_handle, void* metadata, uint32_t* out_transform_hint));
    ANO_USE_SYM_SIGN(12, 13);
    ANO_SYM_SIGN(14, VSP(*)(void*, void* name, uint32_t w, uint32_t h, PixFmt fmt, WFlag flags, void** parent_handle, void* metadata, uint32_t* out_transform_hint));
    ANO_USE_SYM_SIGN(14, 15);
    ANO_USE_SYM_SIGN(15, 16);
    ANO_USE_SYM_SIGN(16, -1);
);

/*     static     */
ANO_DEFINE_SYMBOL(SurfaceComposerClient, openGlobalTransaction,    void(*)());
ANO_DEFINE_SYMBOL(SurfaceComposerClient, closeGlobalTransaction,   void(*)(bool sync));

ANO_DEFINE_SYMBOL(SurfaceControl, setLayer, void*(*)(void*, int32_t z));

ANO_DEFINE_SYMBOL(Surface, disConnect, void*(*)(void*, int api));

ANO_DEFINE_SYMBOL(SurfaceComposerClient_Transaction, apply, int32_t(*)(void*, bool sync),
    ANO_SYM_SIGN(13, int32_t(*)(void*, bool sync, bool one_way));
    ANO_USE_SYM_SIGN(13, 14);
    ANO_USE_SYM_SIGN(14, 15);
    ANO_USE_SYM_SIGN(15, 16);
    ANO_USE_SYM_SIGN(16, -1);
);

ANO_DEFINE_SYMBOL(SurfaceComposerClient_Transaction, setInputWindowInfo, void*(*)(void*, VSP& surface_control, void* input_window_info), 
    ANO_SYM_SIGN(13, void*(*)(void*, VSP& surface_control, void* window_info));
    ANO_USE_SYM_SIGN(13, 14);
    ANO_USE_SYM_SIGN(14, 15);
    ANO_SYM_SIGN(16, void*(*)(void*, VSP& surface_control, void* window_info_handle));
    ANO_USE_SYM_SIGN(16, -1);
);


ANO_DEFINE_SYMBOL(RefBase, incStrong,      void(*)(void*, void* id));
ANO_DEFINE_SYMBOL(RefBase, decStrong,      void(*)(void*, void* id));
ANO_DEFINE_SYMBOL(String8, constructor,    void(*)(void*, const char* const str));
ANO_DEFINE_SYMBOL(String8, destructor,     void(*)(void*));


#undef ANO_USE_SYM_SIGN
#undef ANO_SYM_SIGN
#undef ANO_DEFINE_SYMBOL

    private:
        Symbol() {
            Log::trace("load libgui symbols");
            
            void* libgui = dlopen("libgui.so", RTLD_LAZY);
            if (libgui == nullptr) {
                auto err = "failed to open libgui.so";
                Log::fatal(err);
                throw std::runtime_error(err);
            }
            
            void* libutils = dlopen("libutils.so", RTLD_LAZY);
            if (libutils == nullptr) {
                auto err = "failed to open libutils.so";
                Log::fatal(err);
                throw std::runtime_error(err);
            }
    
            const SVCode system_version = getAndroidVersion();

#define ANO_LOAD_SYM(HANDLE, SPACE, SYMBOL) \
do {                                                                \
    auto str  = String::SPACE##_##SYMBOL.get(system_version);       \
    auto name = #SPACE "::" #SYMBOL;                                \
    if (!str.has_value()) {                                         \
        Log::warn("skip symbol: {}", name);                         \
        break;                                                      \
    }                                                               \
    void* sym = dlsym(HANDLE, str->get().c_str());                  \
    if (sym == nullptr) {                                           \
        Log::error("failed to load symbol: {}", name);              \
        break;                                                      \
    }                                                               \
    Log::trace("load symbol: 0x{:x} - {}", (uintptr_t)sym, name);   \
    Fn_##SPACE##_##SYMBOL.set(sym);                                 \
} while(false)

            ANO_LOAD_SYM(libgui, LayerMetadata, constructor);
            ANO_LOAD_SYM(libgui, LayerMetadata, setInt32);
            ANO_LOAD_SYM(libgui, SurfaceComposerClient, constructor);
            ANO_LOAD_SYM(libgui, SurfaceComposerClient, mirrorSurface);
            ANO_LOAD_SYM(libgui, SurfaceComposerClient, getInternalDisplayToken);
            ANO_LOAD_SYM(libgui, SurfaceComposerClient, getBuiltInDisplay);
            ANO_LOAD_SYM(libgui, SurfaceComposerClient, getDisplayState);
            ANO_LOAD_SYM(libgui, SurfaceComposerClient, getDisplayInfo);
            ANO_LOAD_SYM(libgui, SurfaceComposerClient, getPhysicalDisplayIds);
            ANO_LOAD_SYM(libgui, SurfaceComposerClient, getPhysicalDisplayToken);
            ANO_LOAD_SYM(libgui, SurfaceComposerClient_Transaction, copyConstructor);
            ANO_LOAD_SYM(libgui, SurfaceComposerClient_Transaction, constructor);
            ANO_LOAD_SYM(libgui, SurfaceComposerClient_Transaction, setLayer);
            ANO_LOAD_SYM(libgui, SurfaceComposerClient_Transaction, setTrustedOverlay);
            ANO_LOAD_SYM(libgui, SurfaceComposerClient_Transaction, setLayerStack);
            ANO_LOAD_SYM(libgui, SurfaceComposerClient_Transaction, show);
            ANO_LOAD_SYM(libgui, SurfaceComposerClient_Transaction, hide);
            ANO_LOAD_SYM(libgui, SurfaceComposerClient_Transaction, reparent);
            ANO_LOAD_SYM(libgui, SurfaceComposerClient_Transaction, setMatrix);
            ANO_LOAD_SYM(libgui, SurfaceComposerClient_Transaction, setPosition);
            ANO_LOAD_SYM(libgui, SurfaceControl, getSurface);
            ANO_LOAD_SYM(libgui, SurfaceControl, disConnect);
            ANO_LOAD_SYM(libgui, SurfaceControl, getParentingLayer);
            ANO_LOAD_SYM(libgui, SurfaceComposerClient, createSurface);
            ANO_LOAD_SYM(libgui, SurfaceComposerClient, openGlobalTransaction);
            ANO_LOAD_SYM(libgui, SurfaceComposerClient, closeGlobalTransaction);
            ANO_LOAD_SYM(libgui, SurfaceControl, setLayer);
            ANO_LOAD_SYM(libgui, Surface, disConnect);
            ANO_LOAD_SYM(libgui, SurfaceComposerClient_Transaction, apply);
            ANO_LOAD_SYM(libgui, SurfaceComposerClient_Transaction, setInputWindowInfo);

            ANO_LOAD_SYM(libutils, RefBase, incStrong);
            ANO_LOAD_SYM(libutils, RefBase, decStrong);
            ANO_LOAD_SYM(libutils, String8, constructor);
            ANO_LOAD_SYM(libutils, String8, destructor);

#undef ANO_LOAD_SYM
        }
public:
        static Symbol& getInstance() {
            static Symbol instance;
            return instance;
        }
    };
}
