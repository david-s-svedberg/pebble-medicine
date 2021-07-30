#include "config_menu_window_logic.h"

#include <pebble.h>

#include "edit_alarm_window.h"
#include "format.h"
#include "persistance.h"
#include "icons.h"

static char m_titles[5][6];

static void handle_alarm_edit(int index, void* context)
{
    setup_edit_alarm_window(index);
}

void update_config_menu_items(SimpleMenuItem* menu_items)
{
    AlarmTimeOfDay* alarms = GetAlarms();
    for(int i = 0; i < MAX_ALARMS; i++)
    {
        AlarmTimeOfDay* current = (alarms + i);
        menu_items[i].icon = current->active ? get_alarm_icon() : get_no_alarm_icon();
        menu_items[i].callback = handle_alarm_edit;
        fill_time_string(m_titles[i], current->hour, current->minute);

        menu_items[i].title = m_titles[i];
        menu_items[i].subtitle = current->active ? "Active" : "Not Active";
    }
}