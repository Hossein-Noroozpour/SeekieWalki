#pragma once
#include "../loader.hpp"
#include "physical.hpp"
namespace gearoenix {
    namespace nufrag {
        namespace vulkan {
            namespace device {
                class Logical {
                public:
                    Physical *p = nullptr;
                    VkDevice v;
                    Logical(Physical *p);
                    ~Logical();
                };
            }
        }
    }
}