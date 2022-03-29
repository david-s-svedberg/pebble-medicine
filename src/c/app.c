#include "app.h"

#include "main_window.h"
#include "config_menu_window.h"
#include "edit_alarm_window.h"
#include "alarm_window.h"

#include "icons.h"
#include "app_glance.h"
#include "persistance.h"
#include "scheduler.h"

static void wakeup_handler(WakeupId id, int32_t alarm_index)
{
    setup_alarm_window(alarm_index, get_background_color(), get_foreground_color());
}
static bool first_start()
{
    return !has_any_data();
}

void init()
{
    if(first_start())
    {
        APP_LOG(APP_LOG_LEVEL_INFO, "Removing old wake ups");
        // Mostly for development
        // But old wake ups are not removed when reinstalling app
        // So we remove them so no collisions happen
        wakeup_cancel_all();
    }
    if(launch_reason() == APP_LAUNCH_WAKEUP)
    {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Starting from Wakeup");

        WakeupId id = 0;
        int32_t alarm_index = 0;
        wakeup_get_launch_event(&id, &alarm_index);

        if(alarm_index == SUMMER_TIME_ALARM_ID)
        {
            wakeup_cancel_all();
            ensure_all_alarms_scheduled();
        } else
        {
            setup_alarm_window(alarm_index, get_background_color(), get_foreground_color());
        }
    } else
    {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Starting from non Wakeup");

        wakeup_service_subscribe(wakeup_handler);

        setup_main_window(get_background_color(), get_foreground_color());
    }
}

void deinit()
{
    APP_LOG(APP_LOG_LEVEL_INFO, "Deiniting Meds");

    tear_down_main_window();
    tear_down_config_menu_window();
    tear_down_edit_alarm_window();
    tear_down_alarm_window();
    destroy_all_icons();
    setup_app_glance();
}