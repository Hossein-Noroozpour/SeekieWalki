#pragma once
#include "../core/build-configuration.hpp"
#include "loader.hpp"
#include "instance.hpp"
#ifdef IN_ANDROID
#include <android/native_window.h>
#elif IN_LINUX
#error "It is not implemented yet!"
#elif IN_WINDOWS
#error "It is not implemented yet!"
#else
#error "It is not implemented yet!"
#endif
namespace gearoenix {
    namespace nufrag {
        namespace vulkan {
            class Surface {
            public:
                VkSurfaceKHR v;
#ifdef IN_ANDROID
                Surface(ANativeWindow *w, Loader *l, Instance *i);
#elif IN_LINUX
#error "It is not implemented yet!"
#elif IN_WINDOWS
#error "It is not implemented yet!"
#else
#error "It is not implemented yet!"
#endif
                ~Surface();
            };
        }
    }
}