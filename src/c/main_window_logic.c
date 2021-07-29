#include "main_window_logic.h"

#include "config_menu_window.h"
#include "persistance.h"
#include "format.h"

void take_next_medicine(ClickRecognizerRef recognizer, void* context)
{
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Take next medicine requested");
}

void goto_config_window(ClickRecognizerRef recognizer, void* context)
{
    setup_config_menu_window();
}

void fill_next_alarm_time(char* buf)
{
    AlarmTimeOfDay* next = GetNextAlarmTime();
    if(next == NULL)
    {
        snprintf(buf, sizeof(buf), "None");
    } else
    {
        fill_time_string(buf, next->hour, next->minute);
    }
}
