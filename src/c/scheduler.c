#include "scheduler.h"

#include <pebble.h>

#include "persistance.h"
#include "format.h"

static WakeupId schedule(Alarm* alarm, time_t time)
{
    struct tm* local_scheduled_time = localtime(&time);
    APP_LOG(
        APP_LOG_LEVEL_DEBUG,
        "scheduling alarm for '%02d:%02d' on the %d",
        local_scheduled_time->tm_hour,
        local_scheduled_time->tm_min,
        local_scheduled_time->tm_mday);

    unschedule_alarm(alarm);

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
    snooze_alarm->active = true;
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

static struct tm get_alarm_time_base(Alarm* alarm)
{
    time_t now = time(NULL);
    struct tm* base_alarm_time = localtime(&now);
    base_alarm_time->tm_hour = alarm->time.hour;
    base_alarm_time->tm_min = alarm->time.minute;
    base_alarm_time->tm_sec = 0;

    return *base_alarm_time;
}

void schedule_alarm_for_tomorrow(Alarm* alarm)
{
    struct tm next_alarm = get_alarm_time_base(alarm);
    next_alarm.tm_mday += 1;
    schedule(alarm, mktime(&next_alarm));
}

static bool is_less_than_x_seconds_from_now(int x, struct tm* time_to_check)
{
    time_t now = time(NULL);
    struct tm* local_now = localtime(&now);

    return (mktime(time_to_check) - mktime(local_now) < x);
}

void schedule_alarm(Alarm* alarm)
{
    struct tm next_alarm = get_alarm_time_base(alarm);

    if(is_less_than_x_seconds_from_now(30, &next_alarm))
    {
        next_alarm.tm_mday += 1;
    }

    schedule(alarm, mktime(&next_alarm));
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