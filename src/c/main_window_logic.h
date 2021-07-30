#pragma once

#include <pebble.h>

void take_next_medicine(ClickRecognizerRef recognizer, void* context);
void goto_config_window(ClickRecognizerRef recognizer, void* context);
void update_next_alarm_time();
void set_main_window_layers(TextLayer* next_alarm_layer);