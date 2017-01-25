#pragma once
#include "../core/build-configuration.hpp"
#ifdef LOG_ENABLED
#ifdef IN_ANDROID
#include "android/log.hpp"
#else

#endif // LOG_ENABLED
#else // IN_ANDROID
#endif // IN_ANDROID