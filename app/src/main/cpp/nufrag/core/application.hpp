#pragma once
#include "../render/engine.hpp"
namespace vulkpp {
    namespace core {
        class Application {
            RenderEngine *render_engine;
        public:
            void started(RenderEngine *engine);
            void update();
            void terminate();
        };
    }
}