#include "scheduler.h"

#include <pebble.h>

#include "persistance.h"
#include "format.h"

static WakeupId schedule(Alarm* alarm, time_t time)
{
    struct tm* local_scheduled_time = localtime(&time);
    APP_LOG(
        APP_LOG_LEVEL_DEBUG,
        "scheduling alarm for '%02d:%02d'",
        local_scheduled_time->tm_hour,
        local_scheduled_time->tm_min);

    WakeupId id = E_RANGE;
    do {
        id = wakeup_schedule(time, alarm->index, true);
        if(id == E_RANGE)
        {
            time += SECONDS_PER_MINUTE / 2;
        }
    } while(id == E_RANGE);

    if(id >= 0)
    {
        alarm->wakeup_id = id;
    } else
    {
        switch (id)
        {
        case E_INVALID_ARGUMENT:
            APP_LOG(APP_LOG_LEVEL_ERROR, "ERROR: Time is in the past");
            break;
        case E_OUT_OF_RESOURCES:
            APP_LOG(APP_LOG_LEVEL_ERROR, "ERROR: No more wakups for this app");
            break;
        case E_INTERNAL:
            APP_LOG(APP_LOG_LEVEL_ERROR, "ERROR: system error occurred during scheduling");
            break;
        default:
            break;
        }
    }
    return id;
}

void schedule_snooze_alarm(Alarm* alarm, time_t wakup_time)
{
    Alarm* snooze_alarm = get_snooze_alarm();
    snooze_alarm->time.hour = alarm->time.hour;
    snooze_alarm->time.minute = alarm->time.minute;
    schedule(snooze_alarm, wakup_time);
}

void ensure_all_alarms_scheduled()
{
    for(int i = 0; i < MAX_ALARMS; i++)
    {
        Alarm* alarm = get_alarm(i);
        if(alarm->active)
        {
            bool scheduled = wakeup_query(alarm->wakeup_id, NULL);
            if(!scheduled)
            {
                schedule_alarm(alarm);
            }
        }
    }
    schedule_alarm(get_alarm(SUMMER_TIME_ALARM_ID));
}

void schedule_alarm_for_tomorrow(Alarm* alarm)
{
    time_t t = clock_to_timestamp(TODAY, alarm->time.hour, alarm->time.minute) + SECONDS_PER_DAY;
    schedule(alarm, t);
}

void schedule_alarm(Alarm* alarm)
{
    time_t t = clock_to_timestamp(TODAY, alarm->time.hour, alarm->time.minute);
    time_t now = time(NULL);
    if(t - now < 30)
    {
        t += SECONDS_PER_DAY;
    }
    schedule(alarm, t);
}

void reschedule_alarm(Alarm* alarm)
{
    unschedule_alarm(alarm);
    schedule_alarm(alarm);
}

void unschedule_alarm(Alarm* alarm)
{
    wakeup_cancel(alarm->wakeup_id);
}

void unschedule_all()
{
    wakeup_cancel_all();
}

TimeOfDay get_scheduled_time(Alarm* alarm)
{
    time_t scheduled_time;
    bool isScheduled = wakeup_query(alarm->wakeup_id, &scheduled_time);
    struct tm* local_scheduled_time = localtime(&scheduled_time);
    TimeOfDay scheduled_time_of_day =
    {
        .hour = local_scheduled_time->tm_hour,
        .minute = local_scheduled_time->tm_min
    };
    return scheduled_time_of_day;
}

char* get_next_alarm_time_string()
{
    static char next_alarm_buffer[6];
    Alarm* next = get_next_alarm();
    if(next == NULL)
    {
        snprintf(next_alarm_buffer, 6, "None");
    } else
    {
        TimeOfDay scheduled_time_of_day = get_scheduled_time(next);
        fill_time_string(next_alarm_buffer, scheduled_time_of_day.hour, scheduled_time_of_day.minute);
    }

    return next_alarm_buffer;
}