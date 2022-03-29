#pragma once

#include "data.h"

void schedule_alarm_for_tomorrow(Alarm* alarm);
void schedule_alarm(Alarm* alarm);
void schedule_snooze_alarm(Alarm* alarm, time_t wakup_time);
void reschedule_alarm(Alarm* alarm);
void unschedule_alarm(Alarm* alarm);
void unschedule_all();
void ensure_all_alarms_scheduled();
TimeOfDay get_scheduled_time(Alarm* alarm);
char* get_next_alarm_time_string();