#pragma once

#include <pebble.h>

#include "persistance.h"

void set_alarm_window(Window* alarm_window);
void set_alarm_layers(
    TextLayer* alarm_time_layer,
    TextLayer* snooze_time_layer,
    ActionBarLayer* alarm_window_action_bar_layer);
void setup_alarm_state();
void alarm_window_click_config_provider(void* context);
char* get_wakeup_alarm_time_string();