#pragma once

#include "persistance.h"
#include <pebble.h>

void setup_edit_alarm_window(int edit_alarm_index, GColor8 background_color, GColor8 foreground_color);
void tear_down_edit_alarm_window();