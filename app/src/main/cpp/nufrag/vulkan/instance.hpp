#pragma once
#include "loader.hpp"
namespace gearoenix {
    namespace nufrag {
        namespace vulkan {
            class Instance {
            public:
                Loader *l;
                VkInstance v;
                Instance(Loader *l);
                ~Instance();
            };
        }
    }
}