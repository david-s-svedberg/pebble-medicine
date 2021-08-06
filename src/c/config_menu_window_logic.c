#include "config_menu_window_logic.h"

#include <pebble.h>

#include "edit_alarm_window.h"
#include "format.h"
#include "persistance.h"
#include "icons.h"
#include "scheduler.h"

static char m_titles[5][6];

static void handle_alarm_edit(int index, void* context)
{
    setup_edit_alarm_window(index);
}

void update_config_menu_items(SimpleMenuItem* menu_items)
{
    Alarm* alarms = get_alarms();
    for(int i = 0; i < MAX_ALARMS; i++)
    {
        Alarm* current = (alarms + i);
        menu_items[i].icon = current->active ? get_alarm_icon_trans() : get_no_alarm_icon_trans();
        menu_items[i].callback = handle_alarm_edit;
        fill_time_string(m_titles[i], current->time.hour, current->time.minute);

        menu_items[i].title = m_titles[i];
        menu_items[i].subtitle = current->active ? "Active" : "Not Active";
    }
}

void reset_alarms(int index, void* context)
{
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Reset Alarms requested");
    unschedule_all();
    Alarm* alarms = get_alarms();
    for(int i = 0; i < MAX_ALARMS; i++)
    {
        Alarm* current = (alarms + i);
        if(current->active)
        {
            schedule_alarm(current);
        }
    }
}