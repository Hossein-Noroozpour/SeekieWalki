cmake_minimum_required(VERSION 3.4.1)
set(APP_GLUE_DIR ${ANDROID_NDK}/sources/android/native_app_glue)
include_directories(${APP_GLUE_DIR})
add_library( app-glue STATIC ${APP_GLUE_DIR}/android_native_app_glue.c)
# set(VK_VAL_LAYER_SRC_DIR ${ANDROID_NDK}/sources/third_party/vulkan/src)
set(APP_SRC_DIR src/main/cpp)
add_library(
    main SHARED
    ${APP_SRC_DIR}/gearoenix/nufrag/core/application.cpp
    ${APP_SRC_DIR}/gearoenix/nufrag/core/build-configuration.cpp
    ${APP_SRC_DIR}/gearoenix/nufrag/core/static.cpp
    ${APP_SRC_DIR}/gearoenix/nufrag/core/tree-object.cpp
    ${APP_SRC_DIR}/gearoenix/nufrag/render/engine.cpp
    ${APP_SRC_DIR}/gearoenix/nufrag/system/android/application.cpp
    ${APP_SRC_DIR}/gearoenix/nufrag/system/android/log.cpp
    ${APP_SRC_DIR}/gearoenix/nufrag/system/log.cpp
    ${APP_SRC_DIR}/gearoenix/nufrag/vulkan/device/logical.cpp
    ${APP_SRC_DIR}/gearoenix/nufrag/vulkan/device/physical.cpp
    ${APP_SRC_DIR}/gearoenix/nufrag/vulkan/engine.cpp
    ${APP_SRC_DIR}/gearoenix/nufrag/vulkan/instance.cpp
    ${APP_SRC_DIR}/gearoenix/nufrag/vulkan/loader.cpp
    ${APP_SRC_DIR}/gearoenix/nufrag/vulkan/surface.cpp
)
# include_directories(${VK_VAL_LAYER_SRC_DIR}/include)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Werror -Wno-unused-variable -frtti \
    -fexceptions")
target_link_libraries(main app-glue log android vulkan)
