#pragma once
namespace vulkpp {
    namespace core {
        class Application {
        public:
            virtual void started();
            virtual void update();
            virtual void terminate();
        };
    }
}