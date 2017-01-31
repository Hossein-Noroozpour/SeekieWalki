#include "logical.hpp"
#include "../check.hpp"
#include "../../core/static.hpp"
gearoenix::nufrag::vulkan::device::Logical::Logical(Physical *p): p(p){
    const char *device_extensions[] = {
            "VK_KHR_swapchain"
    };
    float priorities[] = { 1.0f };
    VkDeviceQueueCreateInfo queue_create_info;
    std::memset(&queue_create_info, 0, sizeof(queue_create_info));
    queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_info.queueCount = 1;
    queue_create_info.queueFamilyIndex = 0;
    queue_create_info.pQueuePriorities = priorities;
    VkDeviceCreateInfo device_create_info;
    std::memset(&device_create_info, 0, sizeof(device_create_info));
    device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_create_info.queueCreateInfoCount = 1;
    device_create_info.pQueueCreateInfos = &queue_create_info;
    device_create_info.enabledExtensionCount = countof(device_extensions);
    device_create_info.ppEnabledExtensionNames = device_extensions;
    VKC(p->i->l->vkCreateDevice(p->v, &device_create_info, 0, &v));
}

gearoenix::nufrag::vulkan::device::Logical::~Logical() {

}
