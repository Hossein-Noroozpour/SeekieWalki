#include <android_native_app_glue.h>
#include "../../core/application.hpp"
#include "../log.hpp"
#include <string>
#define CORE_APP nufrag::core::Application

class AppData {
public:
    CORE_APP *core_app;
    RENDER_ENGINE *engine;
};

static void handle_cmd(android_app *app, int32_t cmd) {
    AppData *data = static_cast<AppData *>(app->userData);
    CORE_APP *core_app = data->core_app;
    RENDER_ENGINE *engine = data->engine;
    switch (cmd) {
        case APP_CMD_INIT_WINDOW:
            engine->start(app);
            core_app->start(engine);
            break;
        case APP_CMD_TERM_WINDOW:
            core_app->terminate();
            engine->terminate();
            break;
        default:
            LOGI(std::string("event not handled: ") + std::to_string(cmd));
    }
}

void android_main(struct android_app *app) {
    app_dummy();
    CORE_APP *core_app = new CORE_APP;
    RENDER_ENGINE *engine = new RENDER_ENGINE;
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
