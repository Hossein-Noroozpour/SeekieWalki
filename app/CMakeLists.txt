cmake_minimum_required(VERSION 3.4.1)
set(APP_GLUE_DIR ${ANDROID_NDK}/sources/android/native_app_glue)
include_directories(${APP_GLUE_DIR})
add_library( app-glue STATIC ${APP_GLUE_DIR}/android_native_app_glue.c)
set(VK_VAL_LAYER_SRC_DIR ${ANDROID_NDK}/sources/third_party/vulkan/src)
set(APP_SRC_DIR src/main/cpp)
set(VULKPP_SRC_DIR src/main/cpp/vulkpp)
add_library(
    main SHARED
    ${VULKPP_SRC}/system/android/application.cpp
    ${APP_SRC_DIR}/AndroidMain.cpp
    ${APP_SRC_DIR}/VulkanMain.cpp
    ${APP_SRC_DIR}/vulkan_wrapper.cpp)
include_directories(
    ${VK_VAL_LAYER_SRC_DIR}/include)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Werror       \
    -Wno-unused-variable -DUSE_DEBUG_EXTENTIONS \
    -DVK_USE_PLATFORM_ANDROID_KHR")
target_link_libraries(main app-glue log android)
