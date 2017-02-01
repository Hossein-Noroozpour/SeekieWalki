#pragma once
#include "loader.hpp"
#include "../core/tree-object.hpp"
#include <set>
namespace gearoenix {
    namespace nufrag {
        namespace vulkan {
            class Instance: public core::TreeObject {
            public:
                Loader *l = nullptr;
                VkInstance v;
                Instance(Loader *l, core::TreeObject *p);
                ~Instance();
            };
        }
    }
}