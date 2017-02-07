#pragma once

#include "../loader.hpp"
#include "../instance.hpp"
#include "../surface.hpp"
#include <vector>

namespace gearoenix {
    namespace nufrag {
        namespace vulkan {
            namespace device {
                class Physical {
                public:
                    Instance *i = nullptr;
                    VkPhysicalDevice v;

                    Physical(Instance *i);

                    ~Physical();

                    std::shared_ptr<VkSurfaceCapabilitiesKHR> get_capabilities(Surface *s);

                    std::vector<VkSurfaceFormatKHR> get_formats(Surface *s);
                };
            }
        }
    }
}