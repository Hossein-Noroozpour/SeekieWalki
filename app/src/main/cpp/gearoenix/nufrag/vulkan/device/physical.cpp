#include "physical.hpp"
#include "../check.hpp"

gearoenix::nufrag::vulkan::device::Physical::Physical(Instance *i) : i(i) {
    uint32_t gpu_count = 0;
    VKC(i->l->enumerate_physical_devices(i->v, &gpu_count, nullptr));
    VkPhysicalDevice *gpus = new VkPhysicalDevice[gpu_count];
    VKC(i->l->enumerate_physical_devices(i->v, &gpu_count, gpus));
    v = gpus[0];
    delete[] gpus;
}

gearoenix::nufrag::vulkan::device::Physical::~Physical() {

}

std::shared_ptr<VkSurfaceCapabilitiesKHR>
gearoenix::nufrag::vulkan::device::Physical::get_capabilities(Surface *s) {
    VkSurfaceCapabilitiesKHR *caps = new VkSurfaceCapabilitiesKHR;
    i->l->vkGetPhysicalDeviceSurfaceCapabilitiesKHR(v, s->v, caps);
    return std::shared_ptr<VkSurfaceCapabilitiesKHR>(caps);
}

std::vector<VkSurfaceFormatKHR>
gearoenix::nufrag::vulkan::device::Physical::get_formats(Surface *s) {
    uint32_t count = 0;
    i->l->vkGetPhysicalDeviceSurfaceFormatsKHR(v, s->v, &count, nullptr);
    std::vector<VkSurfaceFormatKHR> formats(count);
    i->l->vkGetPhysicalDeviceSurfaceFormatsKHR(v, s->v, &count, formats.data());
    return formats;
}







