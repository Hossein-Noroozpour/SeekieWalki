#pragma once
#include <android_native_app_glue.h>
#include "loader.hpp"
namespace gearoenix {
    namespace nufrag {
        namespace vulkan {
            class Engine {
            private:
                enum ShaderType {
                    VERTEX_SHADER, FRAGMENT_SHADER
                };
                void create_device(ANativeWindow *platformWindow);
                void create_swapchain();
                void create_framebuffers(VkRenderPass &renderPass, VkImageView depthView = VK_NULL_HANDLE);
                bool map_memory_type_to_index(uint32_t typeBits, VkFlags requirements_mask, uint32_t *typeIndex);
                bool create_buffers();
                void delete_buffers();
                VkResult load_shader_from_file(const char *filePath, VkShaderModule *shaderOut, ShaderType type);
                VkResult create_graphics_pipeline();
                void delete_graphics_pipeline();
                void delete_swapchain();

            public:
                Loader *l = nullptr;

                void start(android_app *app);

                bool ready();

                void update();

                void terminate();
            };
        }
    }
}