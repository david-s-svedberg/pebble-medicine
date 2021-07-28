#include <pebble.h>
#include "settings.h"

#define SETTINGS_KEY 1
#define NONE_VALUE 0
#define SWEDISH_VALUE 1
#define ENGLISH_VALUE 2

const char *swe_days[] = {"Söndag", "Måndag", "Tisdag", "Onsdag", "Torsdag", "Fredag", "Lördag"};
const char *eng_days[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

static struct ClaySettings current_settings = { .Language = NONE_VALUE };

static void load_settings()
{
    if(E_DOES_NOT_EXIST == persist_read_data(SETTINGS_KEY, &current_settings, sizeof(current_settings)))
    {
        current_settings.Language = SWEDISH_VALUE;
    }
}

static struct ClaySettings *get_current_settings()
{
    if(current_settings.Language == NONE_VALUE)
    {
        load_settings();
    }

    return &current_settings;
}

static void save_settings(struct ClaySettings *settings)
{
    persist_write_data(SETTINGS_KEY, settings, sizeof(settings));
}

const char* get_day_name(int dayOfWeek)
{
    const char *ret = NULL;
    struct ClaySettings *settings = get_current_settings();
    if(settings->Language == SWEDISH_VALUE)
    {
        ret = swe_days[dayOfWeek];
    }
    else if(settings->Language == ENGLISH_VALUE)
    {
        ret = eng_days[dayOfWeek];
    }
    else
    {
        ret = NULL;
    }

    return ret;
}

void on_settings_changed(DictionaryIterator *iter, void *context)
{
    Tuple *language_code = dict_find(iter, MESSAGE_KEY_Language);
    struct ClaySettings *settings = get_current_settings();

    if (language_code != NULL)
    {
        settings->Language = language_code->value->int32;
    }

    save_settings(settings);
}