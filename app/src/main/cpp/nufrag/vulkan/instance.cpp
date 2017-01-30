#include "instance.hpp"
#include "check.hpp"
#include "../core/static.hpp"

gearoenix::nufrag::vulkan::Instance::Instance(Loader *l): l(l) {
    VkApplicationInfo app_info;
    std::memset(&app_info, 0, sizeof(app_info));
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.apiVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pApplicationName = "Seeki Walki";
    app_info.pEngineName = "NuFrag";
    const char *instance_extensions[] = {
            "VK_KHR_surface",
            "VK_KHR_android_surface",
    };
    VkInstanceCreateInfo instance_create_info;
    std::memset(&instance_create_info, 0, sizeof(instance_create_info));
    instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_create_info.pApplicationInfo = &app_info;
    instance_create_info.enabledExtensionCount = countof(instance_extensions);
    instance_create_info.ppEnabledExtensionNames = instance_extensions;
    VKC(l->create_instance(&instance_create_info, 0, &v));
}

gearoenix::nufrag::vulkan::Instance::~Instance() {
    l->destroy_instance(v, 0);
}


