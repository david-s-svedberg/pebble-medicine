#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "data.h"

Alarm* get_alarms();
Alarm* get_alarm(int index);
Alarm* get_snooze_alarm();
Alarm* get_next_alarm();

GColor8 get_background_color();
GColor8 get_foreground_color();
bool is_dark_theme();
void toggle_theme();

bool has_any_data();
void save_data();