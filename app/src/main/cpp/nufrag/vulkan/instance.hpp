#pragma once
#include "loader.hpp"
namespace gearoenix {
    namespace nufrag {
        namespace vulkan {
            class Instance {
            public:
                Loader *l = nullptr;
                VkInstance v;
                Instance(Loader *l);
                ~Instance();
            };
        }
    }
}