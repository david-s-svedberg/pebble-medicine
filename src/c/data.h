#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <pebble.h>

#define MAX_ALARMS 5
#define SNOOZED_ALARM_ID 5
#define SUMMER_TIME_ALARM_ID 6

typedef struct {
    uint8_t hour;
    uint8_t minute;
} TimeOfDay;

typedef struct {
    uint8_t index;
    TimeOfDay time;
    bool active;
    WakeupId wakeup_id;
} Alarm;

typedef struct {
    Alarm alarms[MAX_ALARMS];
    Alarm snooze_alarm;
    Alarm summer_time_alarm;
    GColor8 background_color;
    GColor8 foreground_color;
    uint8_t alarm_timeout_sec;
} Data;