#pragma once
#include "../render/engine.hpp"
namespace nufrag {
    namespace core {
        class Application {
            RenderEngine *render_engine;
        public:
            void start(RenderEngine *engine);
            void update();
            void terminate();
        };
    }
}