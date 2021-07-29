#pragma once

#include <pebble.h>

void take_next_medicine(ClickRecognizerRef recognizer, void* context);
void goto_config_window(ClickRecognizerRef recognizer, void* context);
void fill_next_alarm_time(char* buf);