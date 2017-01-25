#pragma once
#include "../../core/build-configuration.hpp"
#include <android/log.h>
#include <exception>

#define LOGI(s) __android_log_print(ANDROID_LOG_INFO, APPLICATION_NAME, "%s ----- in file: %s %d", (s).c_str(), __FILE__, __LINE__);
#define LOGD(s) __android_log_print(ANDROID_LOG_DEBUG, APPLICATION_NAME, "%s ----- in file: %s %d", (s).c_str(), __FILE__, __LINE__);
#define LOGE(s) __android_log_print(ANDROID_LOG_ERROR, APPLICATION_NAME, "%s ----- in file: %s %d", (s).c_str(), __FILE__, __LINE__);
#define LOGF(s) __android_log_print(ANDROID_LOG_FATAL, APPLICATION_NAME, "%s ----- in file: %s %d", (s).c_str(), __FILE__, __LINE__); std::terminate();