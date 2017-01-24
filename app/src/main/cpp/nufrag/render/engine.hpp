#pragma once
#include "../vulkan/engine.hpp"
#define USE_VULKAN
#ifdef USE_VULKAN
#include "../vulkan/engine.hpp"
#define RenderEngine nufrag::vulkan::Engine
#endif

