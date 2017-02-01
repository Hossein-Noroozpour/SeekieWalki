#pragma once
#include "../loader.hpp"
#include "../instance.hpp"
#include "../surface.hpp"
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
                    VkSurfaceCapabilitiesKHR *get_capabilities(Surface *s);
                    VkSurfaceFormatKHR *get_formats(Surface *s);
                };
            }
        }
    }
}