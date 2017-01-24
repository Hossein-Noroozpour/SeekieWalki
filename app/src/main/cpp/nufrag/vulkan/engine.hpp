#pragma once
namespace nufrag {
    namespace vulkan {
        class Engine {
        public:
            void start();

            bool ready();

            void terminate();
        };
    }
}