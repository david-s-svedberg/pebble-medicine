#pragma once

typedef struct ClaySettings {
  int Language;
} ClaySettings;

const char* get_day_name(int dayOfWeek);

void on_settings_changed(DictionaryIterator *iter, void *context);