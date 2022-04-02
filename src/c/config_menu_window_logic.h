#pragma once

#include <pebble.h>

void update_alarm_menu_items(SimpleMenuItem* alarm_items);
void update_theme_menu_item(SimpleMenuItem* theme_item);
void update_timeout_menu_item(SimpleMenuItem* theme_item);

void reset_alarms(int index, void* context);
char* get_current_theme();
char* get_current_timeout();