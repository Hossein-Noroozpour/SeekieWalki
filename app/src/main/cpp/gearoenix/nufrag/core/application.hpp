#pragma once
#include "../render/engine.hpp"
namespace nufrag {
    namespace core {
        class Application {
//            RENDER_ENGINE *render_engine;
        public:
            void start(RENDER_ENGINE *engine);
            void update();
            void terminate();
        };
    }
}