#include "app.h"

#include "main_window.h"
#include "config_menu_window.h"
#include "edit_alarm_window.h"
#include "alarm_window.h"

#include "icons.h"
#include "app_glance.h"

static void wakeup_handler(WakeupId id, int32_t alarm_index)
{
    setup_alarm_window(alarm_index);
}

void init()
{
    if(launch_reason() == APP_LAUNCH_WAKEUP)
    {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Starting from Wakeup");

        WakeupId id = 0;
        int32_t alarm_index = 0;
        wakeup_get_launch_event(&id, &alarm_index);

        setup_alarm_window(alarm_index);
    } else
    {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Starting from non Wakeup");

        wakeup_service_subscribe(wakeup_handler);

        setup_main_window();
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