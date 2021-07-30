#include "scheduler.h"

#include <pebble.h>

static WakeupId schedule(Alarm* alarm, time_t time)
{
    struct tm* local_scheduled_time = localtime(&time);
    APP_LOG(
        APP_LOG_LEVEL_DEBUG,
        "scheduling alarm for '%d:%d'",
        local_scheduled_time->tm_hour,
        local_scheduled_time->tm_min);
    WakeupId id = wakeup_schedule(time, alarm->index, true);
    if(id >= 0)
    {
        alarm->wakeup_id = id;
    } else
    {
        switch (id)
        {
        case E_RANGE:
            APP_LOG(APP_LOG_LEVEL_ERROR, "ERROR: Another event in period");
            break;
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
    schedule(alarm, wakup_time);
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