#include <pebble.h>
#include <math.h>

#include "setup.h"

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
    tear_down_app();
}