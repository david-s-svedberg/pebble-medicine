#pragma once

#include "persistance.h"

void set_layers(
    TextLayer *edit_alarm_active_layer,
    TextLayer *edit_alarm_time_hour_layer,
    TextLayer *edit_alarm_time_minute_layer,
    ActionBarLayer* edit_alarm_action_bar_layer
);
void update_edit_alarm_time_layers();
void set_edit_alarm(AlarmTimeOfDay* edit_alarm);
void edit_alarm_action_bar_click_config_provider(void* context);
AlarmTimeOfDay* get_edit_alarm();
void change_to_init_edit_alarm_actions();