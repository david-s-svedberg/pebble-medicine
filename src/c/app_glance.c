#include "app_glance.h"

#include <pebble.h>

static void set_app_glance(AppGlanceReloadSession *session, size_t limit, void *context)
{
    if (limit < 1) return;

    const AppGlanceSlice entry = (AppGlanceSlice) {
        .layout = {
            .icon = PUBLISHED_ID_APP_GLANCE_ICON
        },
        .expiration_time = APP_GLANCE_SLICE_NO_EXPIRATION
    };

    const AppGlanceResult result = app_glance_add_slice(session, entry);
    if (result != APP_GLANCE_RESULT_SUCCESS) {
        APP_LOG(APP_LOG_LEVEL_ERROR, "AppGlance Error: %d", result);
    }
}

void setup_app_glance()
{
    app_glance_reload(set_app_glance, NULL);
}