#include "application.hpp"
#include <android/log.h>
#include "../../core/application.hpp"

#define CORE_APP Application

void handle_cmd(android_app *app, int32_t cmd) {
    CORE_APP *core_app = static_cast<CORE_APP>(app->userData);
    switch (cmd) {
        case APP_CMD_INIT_WINDOW:
            core_app.started();
            break;
        case APP_CMD_TERM_WINDOW:
            core_app.terminate();
            break;
        default:
            __android_log_print(ANDROID_LOG_INFO, "Vulkan Tutorials", "event not handled: %d", cmd);
    }
}

void android_main(struct android_app *app) {
    app_dummy();
    CORE_APP *core_app = new CORE_APP;
    app->userData = core_app;
    app->onAppCmd = handle_cmd;
    int events;
    android_poll_source *source;
    do {
        if (ALooper_pollAll(core_app->ready() ? 1 : 0, nullptr, &events, (void **) &source) >= 0) {
            if (source != NULL) source->process(app, source);
        }
        if (IsVulkanReady()) {
            VulkanDrawFrame();
        }
    } while (app->destroyRequested == 0);
}
