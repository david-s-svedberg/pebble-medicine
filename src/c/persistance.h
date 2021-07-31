#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "data.h"

Alarm* get_alarms();
Alarm* get_alarm(int index);
Alarm* get_next_alarm();

bool has_any_data();
void save_data();