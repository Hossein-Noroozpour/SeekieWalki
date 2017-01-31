#pragma once
#include "../loader.hpp"
#include "../instance.hpp"
namespace gearoenix {
    namespace nufrag {
        namespace vulkan {
            namespace device {
                class Physical {
                public:
                    Instance *i;
                    VkPhysicalDevice v;
                    Physical(Instance *i, Loader *l);
                    ~Physical();
                };
            }
        }
    }
}