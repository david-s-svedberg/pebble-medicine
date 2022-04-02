#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "data.h"

extern const uint8_t FOUR_MINUTES_IN_SEC;
extern const uint8_t ONE_MINUTES_IN_SEC;

Alarm* get_alarms();
Alarm* get_alarm(int index);
Alarm* get_snooze_alarm();
Alarm* get_next_alarm();

GColor8 get_background_color();
GColor8 get_foreground_color();

uint8_t get_alarm_timeout();
void set_alarm_timeout(uint8_t sec);

bool is_dark_theme();
void toggle_theme();

bool has_any_data();
void save_data();