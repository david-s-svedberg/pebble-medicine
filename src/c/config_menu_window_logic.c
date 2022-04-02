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
    setup_edit_alarm_window(index, get_background_color(), get_foreground_color());
}

void update_alarm_menu_items(SimpleMenuItem* alarm_items)
{
    Alarm* alarms = get_alarms();
    for(int i = 0; i < MAX_ALARMS; i++)
    {
        Alarm* current = (alarms + i);
        alarm_items[i].icon = current->active ? get_alarm_icon_trans() : get_no_alarm_icon_trans();
        alarm_items[i].callback = handle_alarm_edit;
        fill_time_string(m_titles[i], current->time.hour, current->time.minute);

        alarm_items[i].title = m_titles[i];
        alarm_items[i].subtitle = current->active ? "Active" : "Not Active";
    }
}

void update_theme_menu_item(SimpleMenuItem* theme_item)
{
    theme_item->subtitle = get_current_theme();
}

void update_timeout_menu_item(SimpleMenuItem* timeout_item)
{
    timeout_item->subtitle = get_current_timeout();
}

char* get_current_theme()
{
    static char theme_buffer[6];

    char* theme = is_dark_theme() ? "Dark" : "Light";
    strncpy(theme_buffer, theme, 6);

    return theme_buffer;
}

void reset_alarms(int index, void* context)
{
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Reset Alarms requested");
    unschedule_all();
    ensure_all_alarms_scheduled();
}

char* get_current_timeout()
{
    static char timeout_buffer[10];

    uint8_t timeout_min = get_alarm_timeout() / 60;
    snprintf(timeout_buffer, 10, "%d minutes", timeout_min);

    return timeout_buffer;
}