#include "physical.hpp"
#include "../check.hpp"

gearoenix::nufrag::vulkan::device::Physical::Physical(Instance *i): i(i){
    uint32_t gpu_count = 0;
    VKC(i->l->enumerate_physical_devices(i->v, &gpu_count, nullptr));
    VkPhysicalDevice *gpus = new VkPhysicalDevice[gpu_count];
    VKC(i->l->enumerate_physical_devices(i->v, &gpu_count, gpus));
    v = gpus[0];
    delete [] gpus;
}

gearoenix::nufrag::vulkan::device::Physical::~Physical() {

}



