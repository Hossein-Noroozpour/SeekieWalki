#include "application.hpp"
#include <android/log.h>
#include <android_native_app_glue.h>
#include "../../core/application.hpp"
#include "../../render/engine.hpp"

#define CORE_APP Application

static class AppData {
public:
    CORE_APP *core_app;
    RenderEngine *engine;
};

static void handle_cmd(android_app *app, int32_t cmd) {
    AppData *data = static_cast<AppData>(app->userData);
    CORE_APP *core_app = data->core_app;
    RenderEngine *engine = data->engine;
    switch (cmd) {
        case APP_CMD_INIT_WINDOW:
            engine->initialize();
            core_app->start(engine);
            break;
        case APP_CMD_TERM_WINDOW:
            core_app->terminate();
            engine->terminate();
            break;
        default:
            __android_log_print(ANDROID_LOG_INFO, "Vulkan Tutorials", "event not handled: %d", cmd);
    }
}

static void android_main(struct android_app *app) {
    app_dummy();
    CORE_APP *core_app = new CORE_APP;
    RenderEngine *engine = new RenderEngine;
    AppData *data = new AppData;
    data->core_app = core_app;
    data->engine = engine;
    app->userData = data;
    app->onAppCmd = handle_cmd;
    int events;
    android_poll_source *source;
    do {
        if (ALooper_pollAll(engine->ready() ? 1 : 0, nullptr, &events, (void **) &source) >= 0) {
            if (source != NULL)
                source->process(app, source);
        }
        if (engine->ready()) {
            core_app->update();
            engine->update();
        }
    } while (app->destroyRequested == 0);
}
