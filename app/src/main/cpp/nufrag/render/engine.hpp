#pragma once
#include "../core/build-configuration.hpp"
#ifdef USE_VULKAN
#include "../vulkan/engine.hpp"
#define RENDER_ENGINE gearoenix::nufrag::vulkan::Engine
#endif

