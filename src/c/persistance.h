#pragma once

#include <stdint.h>
#include <stdbool.h>

#define MAX_ALARMS 5

typedef struct {
    uint8_t hour;
    uint8_t minute;
    bool active;
} AlarmTimeOfDay;

typedef struct {
    AlarmTimeOfDay alarms [MAX_ALARMS];
} Data;

AlarmTimeOfDay* GetAlarms();
AlarmTimeOfDay* GetAlarm(int index);
AlarmTimeOfDay* GetNextAlarmTime();
void save_data();