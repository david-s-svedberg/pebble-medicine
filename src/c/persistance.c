#include "persistance.h"

#include <pebble.h>
#include <stdbool.h>
#include <gcolor_definitions.h>

const uint8_t FOUR_MINUTES_IN_SEC = 240;
const uint8_t ONE_MINUTES_IN_SEC = 60;

static const uint32_t DATA_KEY = 654654;

static Data m_data;
static bool m_data_loaded = false;

static Alarm create_alarm(uint8_t index)
{
    Alarm alarm;

    alarm.index = index;
    alarm.time.hour = 0;
    alarm.time.minute = 0;
    alarm.active = false;

    return alarm;
}

static void seed_data()
{
    for(uint8_t i = 0; i < MAX_ALARMS; i++)
    {
        m_data.alarms[i] = create_alarm(i);
    }
    m_data.snooze_alarm = create_alarm(SNOOZED_ALARM_ID);
    m_data.summer_time_alarm = create_alarm(SUMMER_TIME_ALARM_ID);
    m_data.summer_time_alarm.time.hour = 3;
    m_data.summer_time_alarm.time.minute = 30;
    m_data.background_color = GColorBlack;
    m_data.foreground_color = GColorWhite;
    m_data.alarm_timeout_sec = ONE_MINUTES_IN_SEC;
    persist_write_data(DATA_KEY, &m_data, sizeof(Data));
}

static Data* get_data()
{
    if(!has_any_data())
    {
        seed_data();
    }
    if(!m_data_loaded)
    {
        persist_read_data(DATA_KEY, &m_data, sizeof(Data));
        m_data_loaded = true;
    }
    return &m_data;
}

Alarm* get_alarms()
{
    return get_data()->alarms;
}

Alarm* get_alarm(int index)
{
    return (get_alarms() + index);
}

static uint32_t minutes_until(uint8_t currentHour, uint8_t currentMinutes, uint8_t targetHour, uint8_t targetMinute)
{
    uint32_t ret = 0;

    if(targetHour > currentHour)
    {
        ret += ((targetHour - currentHour)*MINUTES_PER_HOUR);
        if(targetMinute >= currentMinutes)
        {
            ret += targetMinute - currentMinutes;
        } else
        {
            ret -= currentMinutes - targetMinute;
        }
    } else if(targetHour == currentHour)
    {
        if(targetMinute >= currentMinutes)
        {
            ret += targetMinute - currentMinutes;
        } else
        {
            ret = MINUTES_PER_DAY - (currentMinutes - targetMinute);
        }
    } else
    {
        ret = (HOURS_PER_DAY - currentHour + targetHour) * MINUTES_PER_HOUR;
        if(targetMinute >= currentMinutes)
        {
            ret += targetMinute - currentMinutes;
        } else
        {
            ret -= currentMinutes - targetMinute;
        }
    }

    return ret;
}

static Alarm* get_next_schedueled(Alarm** alarms, size_t number_of_alarms)
{
    time_t now = time(NULL);
    Alarm* next = NULL;
    uint32_t currentlyClosest = 0;
    time_t wakup_time;
    for(size_t i = 0; i < number_of_alarms; i++)
    {
        Alarm* current = *(alarms + i);
        if(current != NULL)
        {
            if(current->active)
            {
                bool scheduled = wakeup_query(current->wakeup_id, &wakup_time);
                if(scheduled)
                {
                    uint32_t minutesUntilAlarm = (wakup_time - now)/SECONDS_PER_MINUTE;
                    if(next == NULL || minutesUntilAlarm < currentlyClosest)
                    {
                        APP_LOG(APP_LOG_LEVEL_DEBUG, "Alarm:%d is currently closest with %d minutes until alarm", current->index, (int)minutesUntilAlarm);
                        next = current;
                        currentlyClosest = minutesUntilAlarm;
                    } else {
                        APP_LOG(APP_LOG_LEVEL_DEBUG, "Alarm:%d is not closer with %d minutes until alarm", current->index, (int)minutesUntilAlarm);
                    }
                } else {
                    APP_LOG(APP_LOG_LEVEL_DEBUG, "Alarm:%d not scheduled", current->index);
                }
            } else {
                APP_LOG(APP_LOG_LEVEL_DEBUG, "Alarm:%d not active", current->index);
            }

        }
    }
    return next;
}

Alarm* get_next_alarm()
{
    Alarm* all_alarms[MAX_ALARMS + 1];

    for(int i = 0; i < MAX_ALARMS ; i++)
    {
        all_alarms[i] = get_alarm(i);
    }
    all_alarms[MAX_ALARMS] = get_snooze_alarm();

    return get_next_schedueled(all_alarms, sizeof(all_alarms)/sizeof(all_alarms[0]));
}

bool has_any_data()
{
    return persist_exists(DATA_KEY);
}

void save_data()
{
    persist_write_data(DATA_KEY, &m_data, sizeof(Data));
}

GColor8 get_background_color()
{
    return get_data()->background_color;
}

GColor8 get_foreground_color()
{
    return get_data()->foreground_color;
}

uint8_t get_alarm_timeout()
{
    return get_data()->alarm_timeout_sec;
}

bool is_dark_theme()
{
    uint8_t background = get_background_color().argb;
    uint8_t black = GColorBlack.argb;
    return (background == black);
}

void toggle_theme()
{
    Data* data = get_data();
    GColor8 previous_background_color = data->background_color;
    GColor8 previous_foreground_color = data->foreground_color;

    data->background_color = previous_foreground_color;
    data->foreground_color = previous_background_color;
    save_data();
}

Alarm* get_snooze_alarm()
{
    return &get_data()->snooze_alarm;
}

void set_alarm_timeout(uint8_t sec)
{
    Data* data = get_data();
    data->alarm_timeout_sec = sec;
    save_data();
}