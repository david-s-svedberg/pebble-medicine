#include "app_glance.h"

#include "scheduler.h"

#include <pebble.h>

static char* get_subtitle(){
    static char subtitle_buffer[17];
    char* next_alarm_string = get_next_alarm_time_string();
    snprintf(subtitle_buffer, 17, "Next dose: %s", next_alarm_string);
    return subtitle_buffer;
}

static void set_app_glance(AppGlanceReloadSession *session, size_t limit, void *context)
{
    if (limit < 1) return;

    const AppGlanceSlice entry = (AppGlanceSlice) {
        .layout = {
            .icon = PUBLISHED_ID_APP_GLANCE_ICON,
            .subtitle_template_string = get_subtitle()
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