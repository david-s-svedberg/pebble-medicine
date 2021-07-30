#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <pebble.h>

#define MAX_ALARMS 5

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
} Data;