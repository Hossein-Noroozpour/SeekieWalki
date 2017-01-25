#pragma once
#include <android_native_app_glue.h>
namespace nufrag {
    namespace vulkan {
        class Engine {
        public:
            void start(android_app* app);

            bool ready();

            void update();

            void terminate();
        };
    }
}