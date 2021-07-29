#include <pebble.h>
#include <math.h>

#include "setup.h"

static void ensure_app_glance()
{
    app_glance_reload(set_app_glance, NULL);
}

int main()
{
    if(launch_reason() == APP_LAUNCH_WAKEUP)
    {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Starting from Wakeup");
        setup_alarm();
    } else
    {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Starting from non Wakeup");
        setup_app();
    }
    app_event_loop();
    ensure_app_glance();
    tear_down_app();
}