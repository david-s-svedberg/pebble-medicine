#include "persistance.h"

#include <pebble.h>
#include <stdbool.h>

static const uint32_t DATA_KEY = 654654;
static Data m_data;

static void seed_data()
{
    for(int i = 0; i < MAX_ALARMS; i++)
    {
        m_data.alarms[i].hour = 0;
        m_data.alarms[i].minute = 0;
        m_data.alarms[i].active = false;
    }
    persist_write_data(DATA_KEY, &m_data, sizeof(Data));
}

AlarmTimeOfDay* GetAlarms()
{
    if(!persist_exists(DATA_KEY))
    {
        seed_data();
    }

    persist_read_data(DATA_KEY, &m_data, sizeof(Data));

    return m_data.alarms;
}

AlarmTimeOfDay* GetAlarm(int index)
{
    return (GetAlarms() + index);
}

static uint32_t MinutesUntil(uint8_t currentHour, uint8_t currentMinutes, uint8_t targetHour, uint8_t targetMinute)
{
    uint32_t ret = 0;

    if(targetHour > currentHour)
    {
        ret += ((targetHour - currentHour)*60);
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
            ret = (24*60) - (currentMinutes - targetMinute);
        }
    } else
    {
        ret = (24 - currentHour + targetHour)*60;
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

AlarmTimeOfDay* GetNextAlarmTime()
{
    time_t t = time(NULL);
    struct tm* tm = localtime(&t);
    uint8_t hour = tm->tm_hour;
    uint8_t minute = tm->tm_min;

    AlarmTimeOfDay* alarms = GetAlarms();
    AlarmTimeOfDay* next = NULL;
    uint32_t currentlyClosest = 0;
    for(int i = 0; i < MAX_ALARMS; i++)
    {
        AlarmTimeOfDay* a = (alarms + i);
        if(a != NULL && a->active)
        {
            uint32_t minutesUntilAlarm = MinutesUntil(hour, minute, a->hour, a->minute);
            if(next == NULL || minutesUntilAlarm < currentlyClosest)
            {
                next = a;
                currentlyClosest = minutesUntilAlarm;
            }
        }
    }
    return next;
}

void ensure_all_alarms_set()
{
    wakeup_cancel_all();
    AlarmTimeOfDay* alarms = GetAlarms();
    time_t now = time(NULL);
    for(int i = 0; i < MAX_ALARMS; i++)
    {
        AlarmTimeOfDay* current = (alarms + i);
        if(current->active)
        {
            time_t t = clock_to_timestamp(TODAY, current->hour, current->minute);
            if(t - now < 30)
            {
                t = t + (SECONDS_PER_DAY);
            }
            WakeupId id = wakeup_schedule(t, i, true);
            if(id < 0)
            {
                switch (id)
                {
                case E_RANGE:
                    APP_LOG(APP_LOG_LEVEL_DEBUG, "ERROR: Another event in period");
                    break;
                case E_INVALID_ARGUMENT:
                    APP_LOG(APP_LOG_LEVEL_DEBUG, "ERROR: Time is in the past");
                    break;
                case E_OUT_OF_RESOURCES:
                    APP_LOG(APP_LOG_LEVEL_DEBUG, "ERROR: No more wakups for this app");
                    break;
                case E_INTERNAL:
                    APP_LOG(APP_LOG_LEVEL_DEBUG, "ERROR: system error occurred during scheduling");
                    break;
                default:
                    break;
                }
            }
        }
    }
}

void save_data()
{
    persist_write_data(DATA_KEY, &m_data, sizeof(Data));
    ensure_all_alarms_set();
}