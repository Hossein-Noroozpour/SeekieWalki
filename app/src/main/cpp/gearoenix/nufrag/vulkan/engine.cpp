#include "engine.hpp"
#include "instance.hpp"
#include "surface.hpp"
#include "device/physical.hpp"
#include "device/logical.hpp"
#include "check.hpp"
#include <vector>
#include <cassert>

// Global Variables ...
struct VulkanDeviceInfo {
    bool initialized_;

    gearoenix::nufrag::vulkan::Instance *instance_ = nullptr;
    gearoenix::nufrag::vulkan::Surface *surface_ = nullptr;
    gearoenix::nufrag::vulkan::device::Physical *gpu = nullptr;
    gearoenix::nufrag::vulkan::device::Logical *dev = nullptr;
    VkQueue queue_;
};
VulkanDeviceInfo device_abs;

struct VulkanSwapchainInfo {
    VkSwapchainKHR swapchain_;
    uint32_t swapchainLength_;

    VkExtent2D displaySize_;
    VkFormat displayFormat_;

    // array of frame buffers and views
    VkFramebuffer *framebuffers_;
    VkImageView *displayViews_;
};
VulkanSwapchainInfo swapchain;

struct VulkanBufferInfo {
    VkBuffer vertexBuf;
};
VulkanBufferInfo buffers;

struct VulkanGfxPipelineInfo {
    VkPipelineLayout layout;
    VkPipelineCache cache;
    VkPipeline pipeline;
};
VulkanGfxPipelineInfo gfxPipeline;

struct VulkanRenderInfo {
    VkRenderPass renderPass_;
    VkCommandPool cmdPool_;
    VkCommandBuffer *cmdBuffer_;
    uint32_t cmdBufferLen_;
    VkSemaphore semaphore_;
    VkFence fence_;
};
VulkanRenderInfo render;
android_app *androidAppCtx = nullptr;

void gearoenix::nufrag::vulkan::Engine::create_device(ANativeWindow *platformWindow) {
    device_abs.instance_ = new Instance(l, nullptr);
    device_abs.surface_ = new Surface(platformWindow, l, device_abs.instance_);
    device_abs.gpu = new device::Physical(device_abs.instance_);
    device_abs.dev = new device::Logical(device_abs.gpu);
    l->vkGetDeviceQueue(device_abs.dev->v, 0, 0, &device_abs.queue_);
}

void gearoenix::nufrag::vulkan::Engine::create_swapchain() {
    LOGI(std::string("->createSwapChain"));
    memset(&swapchain, 0, sizeof(swapchain));

    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    l->vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device_abs.gpu->v, device_abs.surface_->v, &surfaceCapabilities);
    uint32_t formatCount = 0;
    l->vkGetPhysicalDeviceSurfaceFormatsKHR(device_abs.gpu->v, device_abs.surface_->v, &formatCount, nullptr);
    VkSurfaceFormatKHR *formats = new VkSurfaceFormatKHR[formatCount];
    l->vkGetPhysicalDeviceSurfaceFormatsKHR(device_abs.gpu->v, device_abs.surface_->v,
                                         &formatCount, formats);
    LOGI(std::string("Got formats") + std::to_string(formatCount));

    uint32_t chosenFormat;
    for (chosenFormat = 0; chosenFormat < formatCount; chosenFormat++) {
        if (formats[chosenFormat].format == VK_FORMAT_R8G8B8A8_UNORM)
            break;
    }
    assert(chosenFormat < formatCount);

    swapchain.displaySize_ = surfaceCapabilities.currentExtent;
    swapchain.displayFormat_ = formats[chosenFormat].format;

    // **********************************************************
    // Create a swap chain (here we choose the minimum available number of surface
    // in the chain)
    uint32_t queueFamily = 0;
    VkSwapchainCreateInfoKHR swapchainCreateInfo{
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .pNext = nullptr,
            .surface = device_abs.surface_->v,
            .minImageCount = surfaceCapabilities.minImageCount,
            .imageFormat = formats[chosenFormat].format,
            .imageColorSpace = formats[chosenFormat].colorSpace,
            .imageExtent = surfaceCapabilities.currentExtent,
            .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
            .imageArrayLayers = 1,
            .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 1,
            .pQueueFamilyIndices = &queueFamily,
            .presentMode = VK_PRESENT_MODE_FIFO_KHR,
            .oldSwapchain = VK_NULL_HANDLE,
            .clipped = VK_FALSE,
    };
    VKC(l->vkCreateSwapchainKHR(device_abs.dev->v, &swapchainCreateInfo,
                                 nullptr, &swapchain.swapchain_));

    // Get the length of the created swap chain
    VKC(l->vkGetSwapchainImagesKHR(device_abs.dev->v, swapchain.swapchain_,
                                    &swapchain.swapchainLength_, nullptr));
    delete[] formats;
    LOGI(std::string("<-createSwapChain"));
}

void gearoenix::nufrag::vulkan::Engine::create_framebuffers(VkRenderPass &renderPass, VkImageView depthView) {
    uint32_t SwapchainImagesCount = 0;
    VKC(l->vkGetSwapchainImagesKHR(device_abs.dev->v, swapchain.swapchain_,
                                    &SwapchainImagesCount, nullptr));
    VkImage *displayImages = new VkImage[SwapchainImagesCount];
    VKC(l->vkGetSwapchainImagesKHR(device_abs.dev->v, swapchain.swapchain_,
                                    &SwapchainImagesCount, displayImages));

    // create image view for each swapchain image
    swapchain.displayViews_ = new VkImageView[SwapchainImagesCount];
    for (uint32_t i = 0; i < SwapchainImagesCount; i++) {
        VkImageViewCreateInfo viewCreateInfo = {
                .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                .pNext = nullptr,
                .image = displayImages[i],
                .viewType = VK_IMAGE_VIEW_TYPE_2D,
                .format = swapchain.displayFormat_,
                .components = {
                        .r = VK_COMPONENT_SWIZZLE_R,
                        .g = VK_COMPONENT_SWIZZLE_G,
                        .b = VK_COMPONENT_SWIZZLE_B,
                        .a = VK_COMPONENT_SWIZZLE_A,
                },
                .subresourceRange = {
                        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                        .baseMipLevel = 0,
                        .levelCount = 1,
                        .baseArrayLayer = 0,
                        .layerCount = 1,
                },
                .flags = 0,
        };
        VKC(l->vkCreateImageView(device_abs.dev->v, &viewCreateInfo, nullptr,
                                  &swapchain.displayViews_[i]));
    }
    delete[] displayImages;

    // create a framebuffer from each swapchain image
    swapchain.framebuffers_ = new VkFramebuffer[swapchain.swapchainLength_];
    for (uint32_t i = 0; i < swapchain.swapchainLength_; i++) {
        VkImageView attachments[2] = {
                swapchain.displayViews_[i], depthView,
        };
        VkFramebufferCreateInfo fbCreateInfo{
                .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                .pNext = nullptr,
                .renderPass = renderPass,
                .layers = 1,
                .attachmentCount = 1,  // 2 if using depth
                .pAttachments = attachments,
                .width = static_cast<uint32_t>(swapchain.displaySize_.width),
                .height = static_cast<uint32_t>(swapchain.displaySize_.height),
        };
        fbCreateInfo.attachmentCount = (depthView == VK_NULL_HANDLE ? 1 : 2);

        VKC(l->vkCreateFramebuffer(device_abs.dev->v, &fbCreateInfo, nullptr,
                                    &swapchain.framebuffers_[i]));
    }
}

// A helper function
bool gearoenix::nufrag::vulkan::Engine::map_memory_type_to_index(uint32_t typeBits, VkFlags requirements_mask, uint32_t *typeIndex) {
    VkPhysicalDeviceMemoryProperties memoryProperties;
    l->vkGetPhysicalDeviceMemoryProperties(device_abs.gpu->v, &memoryProperties);
    // Search memtypes to find first index with those properties
    for (uint32_t i = 0; i < 32; i++) {
        if ((typeBits & 1) == 1) {
            // Type is available, does it match user properties?
            if ((memoryProperties.memoryTypes[i].propertyFlags &
                 requirements_mask) == requirements_mask) {
                *typeIndex = i;
                return true;
            }
        }
        typeBits >>= 1;
    }
    return false;
}

// Create our vertex buffer
bool gearoenix::nufrag::vulkan::Engine::create_buffers() {
    // -----------------------------------------------
    // Create the triangle vertex buffer

    // Vertex positions
    const float vertexData[] = {
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
    };

    // Create a vertex buffer
    uint32_t queueIdx = 0;
    VkBufferCreateInfo createBufferInfo{
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .pNext = nullptr,
            .size = sizeof(vertexData),
            .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            .flags = 0,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .pQueueFamilyIndices = &queueIdx,
            .queueFamilyIndexCount = 1,
    };

    VKC(l->vkCreateBuffer(device_abs.dev->v, &createBufferInfo, nullptr,
                           &buffers.vertexBuf));

    VkMemoryRequirements memReq;
    l->vkGetBufferMemoryRequirements(device_abs.dev->v, buffers.vertexBuf, &memReq);

    VkMemoryAllocateInfo allocInfo{
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .pNext = nullptr,
            .allocationSize = sizeof(vertexData),
            .memoryTypeIndex = 0,  // Memory type assigned in the next step
    };

    // Assign the proper memory type for that buffer
    allocInfo.allocationSize = memReq.size;
    map_memory_type_to_index(memReq.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &allocInfo.memoryTypeIndex);

    // Allocate memory for the buffer
    VkDeviceMemory deviceMemory;
    VKC(l->vkAllocateMemory(device_abs.dev->v, &allocInfo, nullptr, &deviceMemory));

    void *data;
    VKC(l->vkMapMemory(device_abs.dev->v, deviceMemory, 0, sizeof(vertexData), 0,
                        &data));
    memcpy(data, vertexData, sizeof(vertexData));
    l->vkUnmapMemory(device_abs.dev->v, deviceMemory);

    VKC(l->vkBindBufferMemory(device_abs.dev->v, buffers.vertexBuf, deviceMemory, 0));
    return true;
}

void gearoenix::nufrag::vulkan::Engine::delete_buffers() {
    l->vkDestroyBuffer(device_abs.dev->v, buffers.vertexBuf, nullptr);
}

VkResult gearoenix::nufrag::vulkan::Engine::load_shader_from_file(const char *filePath, VkShaderModule *shaderOut, ShaderType) {
    // Read the file
    assert(androidAppCtx);
    AAsset *file = AAssetManager_open(androidAppCtx->activity->assetManager,
                                      filePath, AASSET_MODE_BUFFER);
    off_t fileLength = AAsset_getLength(file);

    char *fileContent = new char[fileLength];

    AAsset_read(file, fileContent, (size_t) fileLength);

    VkShaderModuleCreateInfo shaderModuleCreateInfo{
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .pNext = nullptr,
            .codeSize = (size_t) fileLength,
            .pCode = (const uint32_t *) fileContent,
            .flags = 0,
    };
    VkResult result = l->vkCreateShaderModule(
            device_abs.dev->v, &shaderModuleCreateInfo, nullptr, shaderOut);
    assert(result == VK_SUCCESS);

    delete[] fileContent;

    return result;
}

VkResult gearoenix::nufrag::vulkan::Engine::create_graphics_pipeline() {
    memset(&gfxPipeline, 0, sizeof(gfxPipeline));
    // Create pipeline layout (empty)
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .setLayoutCount = 0,
            .pSetLayouts = nullptr,
            .pushConstantRangeCount = 0,
            .pPushConstantRanges = nullptr,
    };
    VKC(l->vkCreatePipelineLayout(device_abs.dev->v, &pipelineLayoutCreateInfo, nullptr, &gfxPipeline.layout));

    // No dynamic state in that tutorial
    VkPipelineDynamicStateCreateInfo dynamicStateInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            .pNext = nullptr,
            .dynamicStateCount = 0,
            .pDynamicStates = nullptr};

    VkShaderModule vertexShader, fragmentShader;
    load_shader_from_file("shaders/tri.vert.spv", &vertexShader, VERTEX_SHADER);
    load_shader_from_file("shaders/tri.frag.spv", &fragmentShader, FRAGMENT_SHADER);

    // Specify vertex and fragment shader stages
    VkPipelineShaderStageCreateInfo shaderStages[2]{
            {
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                    .pNext = nullptr,
                    .stage = VK_SHADER_STAGE_VERTEX_BIT,
                    .module = vertexShader,
                    .pSpecializationInfo = nullptr,
                    .flags = 0,
                    .pName = "main",
            },
            {
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                    .pNext = nullptr,
                    .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
                    .module = fragmentShader,
                    .pSpecializationInfo = nullptr,
                    .flags = 0,
                    .pName = "main",
            }
    };

    VkViewport viewports{
            .minDepth = 0.0f,
            .maxDepth = 1.0f,
            .x = 0,
            .y = 0,
            .width = (float) swapchain.displaySize_.width,
            .height = (float) swapchain.displaySize_.height,
    };

    VkRect2D scissor = {
            .extent = swapchain.displaySize_,
            .offset = {.x = 0, .y = 0,}
    };
    // Specify viewport info
    VkPipelineViewportStateCreateInfo viewportInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            .pNext = nullptr,
            .viewportCount = 1,
            .pViewports = &viewports,
            .scissorCount = 1,
            .pScissors = &scissor,
    };

    // Specify multisample info
    VkSampleMask sampleMask = ~0u;
    VkPipelineMultisampleStateCreateInfo multisampleInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .pNext = nullptr,
            .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
            .sampleShadingEnable = VK_FALSE,
            .minSampleShading = 0,
            .pSampleMask = &sampleMask,
            .alphaToCoverageEnable = VK_FALSE,
            .alphaToOneEnable = VK_FALSE,
    };

    // Specify color blend state
    VkPipelineColorBlendAttachmentState attachmentStates{
            .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                              VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
            .blendEnable = VK_FALSE,
    };
    VkPipelineColorBlendStateCreateInfo colorBlendInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .pNext = nullptr,
            .logicOpEnable = VK_FALSE,
            .logicOp = VK_LOGIC_OP_COPY,
            .attachmentCount = 1,
            .pAttachments = &attachmentStates,
            .flags = 0,
    };

    // Specify rasterizer info
    VkPipelineRasterizationStateCreateInfo rasterInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .pNext = nullptr,
            .depthClampEnable = VK_FALSE,
            .rasterizerDiscardEnable = VK_FALSE,
            .polygonMode = VK_POLYGON_MODE_FILL,
            .cullMode = VK_CULL_MODE_NONE,
            .frontFace = VK_FRONT_FACE_CLOCKWISE,
            .depthBiasEnable = VK_FALSE,
            .lineWidth = 1,
    };

    // Specify input assembler state
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .pNext = nullptr,
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            .primitiveRestartEnable = VK_FALSE,
    };

    // Specify vertex input state
    VkVertexInputBindingDescription vertex_input_bindings{
            .binding = 0,
            .stride = 3 * sizeof(float),
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
    };
    VkVertexInputAttributeDescription vertex_input_attributes[1]{
            {
                    .binding = 0,
                    .location = 0,
                    .format = VK_FORMAT_R32G32B32_SFLOAT,
                    .offset = 0,
            }
    };
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .pNext = nullptr,
            .vertexBindingDescriptionCount = 1,
            .pVertexBindingDescriptions = &vertex_input_bindings,
            .vertexAttributeDescriptionCount = 1,
            .pVertexAttributeDescriptions = vertex_input_attributes,
    };

    // Create the pipeline cache
    VkPipelineCacheCreateInfo pipelineCacheInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
            .pNext = nullptr,
            .initialDataSize = 0,
            .pInitialData = nullptr,
            .flags = 0,  // reserved, must be 0
    };

    VKC(l->vkCreatePipelineCache(device_abs.dev->v, &pipelineCacheInfo, nullptr,
                                  &gfxPipeline.cache));

    // Create the pipeline
    VkGraphicsPipelineCreateInfo pipelineCreateInfo{
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .stageCount = 2,
            .pStages = shaderStages,
            .pVertexInputState = &vertexInputInfo,
            .pInputAssemblyState = &inputAssemblyInfo,
            .pTessellationState = nullptr,
            .pViewportState = &viewportInfo,
            .pRasterizationState = &rasterInfo,
            .pMultisampleState = &multisampleInfo,
            .pDepthStencilState = nullptr,
            .pColorBlendState = &colorBlendInfo,
            .pDynamicState = &dynamicStateInfo,
            .layout = gfxPipeline.layout,
            .renderPass = render.renderPass_,
            .subpass = 0,
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = 0,
    };

    VkResult pipelineResult =
            l->vkCreateGraphicsPipelines(device_abs.dev->v, gfxPipeline.cache, 1,
                                      &pipelineCreateInfo, nullptr, &gfxPipeline.pipeline);

    // We don't need the shaders anymore, we can release their memory
    l->vkDestroyShaderModule(device_abs.dev->v, vertexShader, nullptr);
    l->vkDestroyShaderModule(device_abs.dev->v, fragmentShader, nullptr);

    return pipelineResult;
}

void gearoenix::nufrag::vulkan::Engine::delete_graphics_pipeline() {
    if (gfxPipeline.pipeline == VK_NULL_HANDLE)
        return;
    l->vkDestroyPipeline(device_abs.dev->v, gfxPipeline.pipeline, nullptr);
    l->vkDestroyPipelineCache(device_abs.dev->v, gfxPipeline.cache, nullptr);
    l->vkDestroyPipelineLayout(device_abs.dev->v, gfxPipeline.layout, nullptr);
}

void gearoenix::nufrag::vulkan::Engine::delete_swapchain() {
    for (int i = 0; i < swapchain.swapchainLength_; i++) {
        l->vkDestroyFramebuffer(device_abs.dev->v, swapchain.framebuffers_[i], nullptr);
        l->vkDestroyImageView(device_abs.dev->v, swapchain.displayViews_[i], nullptr);
    }
    delete[] swapchain.framebuffers_;
    delete[] swapchain.displayViews_;

    l->vkDestroySwapchainKHR(device_abs.dev->v, swapchain.swapchain_, nullptr);
}

void gearoenix::nufrag::vulkan::Engine::start(android_app *app) {
    androidAppCtx = app;
    l = new Loader;
    create_device(app->window);

    create_swapchain();

    // -----------------------------------------------------------------
    // Create render pass
    VkAttachmentDescription attachmentDescriptions{
            .format = swapchain.displayFormat_,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };

    VkAttachmentReference colourReference = {
            .attachment = 0, .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};

    VkSubpassDescription subpassDescription{
            .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
            .flags = 0,
            .inputAttachmentCount = 0,
            .pInputAttachments = nullptr,
            .colorAttachmentCount = 1,
            .pColorAttachments = &colourReference,
            .pResolveAttachments = nullptr,
            .pDepthStencilAttachment = nullptr,
            .preserveAttachmentCount = 0,
            .pPreserveAttachments = nullptr,
    };
    VkRenderPassCreateInfo renderPassCreateInfo{
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
            .pNext = nullptr,
            .attachmentCount = 1,
            .pAttachments = &attachmentDescriptions,
            .subpassCount = 1,
            .pSubpasses = &subpassDescription,
            .dependencyCount = 0,
            .pDependencies = nullptr,
    };
    VKC(l->create_render_pass(device_abs.dev->v, &renderPassCreateInfo,
                               nullptr, &render.renderPass_));

    // -----------------------------------------------------------------
    // Create 2 frame buffers.
    create_framebuffers(render.renderPass_);

    create_buffers();    // create vertex buffers

    // Create graphics pipeline
    create_graphics_pipeline();

    // -----------------------------------------------
    // Create a pool of command buffers to allocate command buffer from
    VkCommandPoolCreateInfo cmdPoolCreateInfo{
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .pNext = nullptr,
            .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            .queueFamilyIndex = 0,
    };
    VKC(l->vkCreateCommandPool(device_abs.dev->v, &cmdPoolCreateInfo,
                                nullptr, &render.cmdPool_));

    // Record a command buffer that just clear the screen
    // 1 command buffer draw in 1 framebuffer
    // In our case we need 2 command as we have 2 framebuffer
    render.cmdBufferLen_ = swapchain.swapchainLength_;
    render.cmdBuffer_ = new VkCommandBuffer[swapchain.swapchainLength_];
    VkCommandBufferAllocateInfo cmdBufferCreateInfo{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .pNext = nullptr,
            .commandPool = render.cmdPool_,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = render.cmdBufferLen_,
    };
    VKC(l->vkAllocateCommandBuffers(device_abs.dev->v,
                                     &cmdBufferCreateInfo,
                                     render.cmdBuffer_));

    for (int bufferIndex = 0; bufferIndex < swapchain.swapchainLength_;
         bufferIndex++) {
        // We start by creating and declare the "beginning" our command buffer
        VkCommandBufferBeginInfo cmdBufferBeginInfo{
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                .pNext = nullptr,
                .flags = 0,
                .pInheritanceInfo = nullptr,
        };
        VKC(l->vkBeginCommandBuffer(render.cmdBuffer_[bufferIndex],
                                     &cmdBufferBeginInfo));

        // Now we start a renderpass. Any draw command has to be recorded in a
        // renderpass
        VkClearValue clearVals{
                .color.float32[0] = 0.0f,
                .color.float32[1] = 0.34f,
                .color.float32[2] = 0.90f,
                .color.float32[3] = 1.0f,
        };
        VkRenderPassBeginInfo renderPassBeginInfo{
                .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
                .pNext = nullptr,
                .renderPass = render.renderPass_,
                .framebuffer = swapchain.framebuffers_[bufferIndex],
                .renderArea = {
                        .offset = {.x = 0, .y = 0,},
                        .extent = swapchain.displaySize_
                },
                .clearValueCount = 1,
                .pClearValues = &clearVals
        };
        l->vkCmdBeginRenderPass(render.cmdBuffer_[bufferIndex], &renderPassBeginInfo,
                             VK_SUBPASS_CONTENTS_INLINE);
        // Bind what is necessary to the command buffer
        l->vkCmdBindPipeline(render.cmdBuffer_[bufferIndex], VK_PIPELINE_BIND_POINT_GRAPHICS,
                          gfxPipeline.pipeline);
        VkDeviceSize offset = 0;
        l->vkCmdBindVertexBuffers(render.cmdBuffer_[bufferIndex], 0, 1, &buffers.vertexBuf,
                               &offset);

        // Draw Triangle
        l->vkCmdDraw(render.cmdBuffer_[bufferIndex], 3, 1, 0, 0);

        l->vkCmdEndRenderPass(render.cmdBuffer_[bufferIndex]);
        VKC(l->vkEndCommandBuffer(render.cmdBuffer_[bufferIndex]));
    }

    // We need to create a fence to be able, in the main loop, to wait for our
    // draw command(s) to finish before swapping the framebuffers
    VkFenceCreateInfo fenceCreateInfo{
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
    };
    VKC(l->vkCreateFence(device_abs.dev->v, &fenceCreateInfo,
                          nullptr, &render.fence_));

    // We need to create a semaphore to be able to wait, in the main loop, for our
    // framebuffer to be available for us before drawing.
    VkSemaphoreCreateInfo semaphoreCreateInfo{
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
    };
    VKC(l->vkCreateSemaphore(device_abs.dev->v, &semaphoreCreateInfo,
                              nullptr, &render.semaphore_));

    device_abs.initialized_ = true;
}

bool gearoenix::nufrag::vulkan::Engine::ready() {
    return device_abs.initialized_;
}

void gearoenix::nufrag::vulkan::Engine::update() {
    uint32_t nextIndex;
    // Get the framebuffer index we should draw in
    VKC(l->vkAcquireNextImageKHR(device_abs.dev->v, swapchain.swapchain_,
                                  UINT64_MAX, render.semaphore_,
                                  VK_NULL_HANDLE, &nextIndex));
    VKC(l->vkResetFences(device_abs.dev->v, 1, &render.fence_));
    VkSubmitInfo submit_info = {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .pNext = nullptr,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &render.semaphore_,
            .commandBufferCount = 1,
            .pCommandBuffers = &render.cmdBuffer_[nextIndex],
            .signalSemaphoreCount = 0,
            .pSignalSemaphores = nullptr
    };
    VKC(l->vkQueueSubmit(device_abs.queue_, 1, &submit_info, render.fence_));
    VKC(l->vkWaitForFences(device_abs.dev->v, 1, &render.fence_, VK_TRUE, 100000000));

    LOGI(std::string("Drawing frames......"));

    VkResult result;
    VkPresentInfoKHR presentInfo{
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .pNext = nullptr,
            .swapchainCount = 1,
            .pSwapchains = &swapchain.swapchain_,
            .pImageIndices = &nextIndex,
            .waitSemaphoreCount = 0,
            .pWaitSemaphores = nullptr,
            .pResults = &result,
    };
    l->vkQueuePresentKHR(device_abs.queue_, &presentInfo);
}

void gearoenix::nufrag::vulkan::Engine::terminate() {
    l->vkFreeCommandBuffers(device_abs.dev->v, render.cmdPool_, render.cmdBufferLen_, render.cmdBuffer_);
    delete[] render.cmdBuffer_;
    l->vkDestroyCommandPool(device_abs.dev->v, render.cmdPool_, nullptr);
    l->vkDestroyRenderPass(device_abs.dev->v, render.renderPass_, nullptr);
    delete_swapchain();
    delete_graphics_pipeline();
    delete_buffers();
    l->vkDestroyDevice(device_abs.dev->v, nullptr);
    delete device_abs.instance_;
    device_abs.initialized_ = false;
}