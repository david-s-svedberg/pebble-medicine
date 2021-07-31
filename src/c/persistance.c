#include "persistance.h"

#include <pebble.h>
#include <stdbool.h>

static const uint32_t DATA_KEY = 654654;
static Data m_data;
static bool m_data_loaded = false;

static void seed_data()
{
    for(uint8_t i = 0; i < MAX_ALARMS; i++)
    {
        m_data.alarms[i].index = i;
        m_data.alarms[i].time.hour = 0;
        m_data.alarms[i].time.minute = 0;
        m_data.alarms[i].active = false;
    }
    persist_write_data(DATA_KEY, &m_data, sizeof(Data));
}

Alarm* get_alarms()
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

    return m_data.alarms;
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

Alarm* get_next_alarm()
{
    time_t now = time(NULL);
    // time_t t = time(NULL);
    // struct tm* tm = localtime(&t);
    // uint8_t hour = tm->tm_hour;
    // uint8_t minute = tm->tm_min;

    Alarm* alarms = get_alarms();
    Alarm* next = NULL;
    uint32_t currentlyClosest = 0;
    time_t wakup_time;
    for(int i = 0; i < MAX_ALARMS; i++)
    {
        Alarm* current = (alarms + i);
        if(current != NULL && current->active)
        {
            wakeup_query(current->wakeup_id, &wakup_time);

            uint32_t minutesUntilAlarm = (wakup_time - now)/SECONDS_PER_MINUTE;
            // uint32_t minutesUntilAlarm = minutes_until(hour, minute, current->hour, current->minute);
            if(next == NULL || minutesUntilAlarm < currentlyClosest)
            {
                next = current;
                currentlyClosest = minutesUntilAlarm;
            }
        }
    }
    return next;
}

bool has_any_data()
{
    return persist_exists(DATA_KEY);
}

void save_data()
{
    persist_write_data(DATA_KEY, &m_data, sizeof(Data));
}