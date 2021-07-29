#include "app.h"

#include "main_window.h"
#include "config_menu_window.h"
#include "edit_alarm_window.h"

#include "icons.h"
#include "app_glance.h"


void init()
{
    if(launch_reason() == APP_LAUNCH_WAKEUP)
    {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Starting from Wakeup");
        setup_alarm();
    } else
    {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Starting from non Wakeup");
        setup_main_window();
    }
}

void deinit()
{
    tear_down_main_window();
    tear_down_config_menu_window();
    tear_down_edit_alarm_window();
    destroy_all_icons();
    setup_app_glance();
}