#include "surface.hpp"
#include "check.hpp"
#include <cstring>
gearoenix::nufrag::vulkan::Surface::Surface(ANativeWindow *w, Loader *l, Instance *i) {
    VkAndroidSurfaceCreateInfoKHR create_info;
    std::memset(&create_info, 0, sizeof(create_info));
    create_info.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
    create_info.window = w;
    VKC(l->vkCreateAndroidSurfaceKHR(i->v, &create_info, 0, &v));
}

gearoenix::nufrag::vulkan::Surface::~Surface() {

}



