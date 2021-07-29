#include "config_menu_window_logic.h"

#include <pebble.h>

#include "edit_alarm_window.h"
#include "format.h"
#include "persistance.h"
#include "icons.h"

static char m_titles[5][6];

static void handle_alarm_edit(int index, void* context)
{
    AlarmTimeOfDay* edit_alarm = GetAlarm(index);
    setup_edit_alarm_window(edit_alarm);
    edit_alarm_window = window_create();

    window_set_window_handlers(edit_alarm_window, (WindowHandlers) {
        .load = setup_edit_alarm_window,
        .unload = tear_down_edit_alarm_window,
        .appear = update_edit_alarm_time_layers
    });

    window_stack_push(edit_alarm_window, true);
}

static void set_menu_item_title(int index, AlarmTimeOfDay* current)
{
    fill_time_string(m_titles[index], current->hour, current->minute);
}

void update_config_menu_items(SimpleMenuItem* menu_items)
{
    AlarmTimeOfDay* alarms = GetAlarms();
    for(int i = 0; i < MAX_ALARMS; i++)
    {
        AlarmTimeOfDay* current = (alarms + i);
        menu_items[i].icon = current->active ? get_alarm_icon() : get_no_alarm_icon();
        menu_items[i].callback = handle_alarm_edit;
        set_menu_item_title(i, current);

        menu_items[i].title = m_titles[i];
        menu_items[i].subtitle = current->active ? "Active" : "Not Active";
    }
}