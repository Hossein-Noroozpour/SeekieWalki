#include "physical.hpp"
#include "../check.hpp"

gearoenix::nufrag::vulkan::device::Physical::Physical(Instance *i, Loader *l) {
    uint32_t gpu_count = 0;
    VKC(l->enumerate_physical_devices(i->v, &gpu_count, nullptr));
    VkPhysicalDevice *gpus = new VkPhysicalDevice[gpu_count];
    VKC(l->enumerate_physical_devices(i->v, &gpu_count, gpus));
    v = gpus[0];
    delete [] gpus;
}

gearoenix::nufrag::vulkan::device::Physical::~Physical() {

}



