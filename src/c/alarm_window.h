#pragma once

#include <stdint.h>
#include <pebble.h>

void setup_alarm_window(int32_t alarm_index, GColor8 background_color, GColor8 foreground_color);
void tear_down_alarm_window();