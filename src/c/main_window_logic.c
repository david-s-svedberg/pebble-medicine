#include "main_window_logic.h"

#include "config_menu_window.h"
#include "persistance.h"
#include "format.h"
#include "scheduler.h"

static TextLayer* m_next_alarm_layer;

// static void fill_next_alarm_time(char* buf)
// {
//     Alarm* next = get_next_alarm();
//     if(next == NULL)
//     {
//         snprintf(buf, 6, "None");
//     } else
//     {
//         TimeOfDay scheduled_time_of_day = get_scheduled_time(next);
//         fill_time_string(buf, scheduled_time_of_day.hour, scheduled_time_of_day.minute);
//     }
// }

void take_next_medicine(ClickRecognizerRef recognizer, void* context)
{
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Take next medicine requested");
    vibes_long_pulse();
    Alarm* next = get_next_alarm();
    if(next != NULL)
    {
        unschedule_alarm(next);
        schedule_alarm_for_tomorrow(next);
        save_data();
        update_next_alarm_time();
    }
}

void goto_config_window(ClickRecognizerRef recognizer, void* context)
{
    setup_config_menu_window(get_background_color(), get_foreground_color());
}

void update_next_alarm_time()
{
    text_layer_set_text(m_next_alarm_layer, get_next_alarm_time_string());
}

void set_main_window_layers(TextLayer* next_alarm_layer)
{
    m_next_alarm_layer = next_alarm_layer;
}