#include "format.h"

#include <stdio.h>

static const char* singleInt = "0%d";
static const char* doubleInt = "%d";

static char* single_hour_single_minute = "0%d:0%d";
static char* single_hour_double_minute = "0%d:%d";
static char* double_hour_double_minute = "%d:%d";
static char* double_hour_single_minute = "%d:0%d";

void fill_time_unit_string(char* buf, uint8_t timeUnit)
{
    snprintf(buf, 3, timeUnit < 10 ? singleInt : doubleInt, timeUnit);
}

void fill_time_string(char* buf, uint8_t hour, uint8_t minute)
{
    char* format = NULL;
    if(hour > 9)
    {
        if(minute > 9)
        {
            format = double_hour_double_minute;
        } else
        {
            format = double_hour_single_minute;
        }
    } else
    {
        if(minute > 9)
        {
            format = single_hour_double_minute;
        } else
        {
            format = single_hour_single_minute;
        }
    }
    snprintf(buf, 6, format, hour, minute);
}