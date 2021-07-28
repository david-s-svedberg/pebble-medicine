#include <pebble.h>
#include <math.h>

#include "update.h"
#include "settings.h"

static TextLayer *time_layer;
static TextLayer *week_numer_layer;
static TextLayer *week_day_layer;
static TextLayer *date_layer;
static TextLayer *steps_layer;

static Layer *battery_layer;

static int batteryIndicatorY = -1;
static int batteryIndicatorInitialX = -1;

static int lastWeekNumber = -1;
static int lastDayNumber = -1;
static int lastHourNumber = -1;

static int currentBatteryLevel = 10;
static int currentSteps = 0;

void init_update_layers(TextLayer *time_layer_p, TextLayer *week_numer_layer_p, TextLayer *week_day_layer_p, TextLayer *date_layer_p, Layer *battery_layer_p, TextLayer *steps_layer_p)
{
    time_layer = time_layer_p;
    week_numer_layer = week_numer_layer_p;
    week_day_layer = week_day_layer_p;
    date_layer = date_layer_p;
    battery_layer = battery_layer_p;
    steps_layer = steps_layer_p;
}

void init_battery_indicator(GRect batteryLayerBounds)
{
    batteryIndicatorY = batteryLayerBounds.origin.y + (batteryLayerBounds.size.h/2);
    batteryIndicatorInitialX = batteryLayerBounds.origin.x + batteryLayerBounds.size.w - 10;
}

static int get_current_week_number(struct tm *tick_time)
{
    char week_number_buffer[3];
    strftime(week_number_buffer, sizeof(week_number_buffer), "%V", tick_time);

    return atoi(week_number_buffer);
}

static void update_time(struct tm *tick_time)
{
    static char time_buffer[8];

    strftime(time_buffer, sizeof(time_buffer), clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time);
    text_layer_set_text(time_layer, time_buffer);
}

static void update_date(struct tm *tick_time)
{
    static char date_buffer[11];

    strftime(date_buffer, sizeof(date_buffer), "%Y-%m-%d", tick_time);
    text_layer_set_text(date_layer, date_buffer);
}

static void update_week_day(struct tm *tick_time)
{
    text_layer_set_text(week_day_layer, get_day_name(tick_time->tm_wday));
}

static void update_week(int currentWeekNumber)
{
    static char week_text_buffer[5];

    snprintf(week_text_buffer, sizeof(week_text_buffer), "v.%d", currentWeekNumber);
    text_layer_set_text(week_numer_layer, week_text_buffer);
}

#if defined(PBL_HEALTH)
static void update_current_steps()
{
    static char steps_text_buffer[16];

    snprintf(steps_text_buffer, sizeof(steps_text_buffer), "%d", currentSteps);
    text_layer_set_text(steps_layer, steps_text_buffer);
}
#endif

void update_battery_layer(Layer *layer, GContext* ctx)
{
    static int xOffset = 5;
    static int yOffset = 4;
    static int radius = 2;

    graphics_context_set_fill_color(ctx, GColorClear);
    GPoint p = {.x = 0, .y = 0};
    for(int i = 0; i < currentBatteryLevel; i++)
    {
        p.x = batteryIndicatorInitialX - (i*xOffset);
        p.y = i % 2 == 0 ? batteryIndicatorY + yOffset : batteryIndicatorY;
        graphics_fill_circle(ctx, p, radius);
    }
}

void set_current_battery_level()
{
    on_battery_state_changed(battery_state_service_peek());
}

void set_current_steps()
{
    currentSteps = (int)health_service_sum_today(HealthMetricStepCount);
}

#if defined(PBL_HEALTH)
void on_health_event(HealthEventType event, void *context)
{
    if(event == HealthEventMovementUpdate)
    {
        set_current_steps();
        update_current_steps();
    }
}
#endif

void on_battery_state_changed(BatteryChargeState charge_state)
{
    currentBatteryLevel = round(charge_state.charge_percent/10);
}

void on_time_tick(struct tm *tick_time, TimeUnits units_changed)
{
    update_time(tick_time);

    int currentHourNumber = tick_time->tm_hour;
    if (lastHourNumber != currentHourNumber)
    {
        lastHourNumber = currentHourNumber;

        int currentDayNumber = tick_time->tm_yday;
        if(lastDayNumber != currentDayNumber)
        {
            lastDayNumber = currentDayNumber;

            update_date(tick_time);
            update_week_day(tick_time);

            int currentWeekNumber = get_current_week_number(tick_time);
            if(lastWeekNumber != currentWeekNumber)
            {
                lastWeekNumber = currentWeekNumber;

                update_week(currentWeekNumber);
            }
        }
    }
}

void update_all()
{
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);
    set_current_battery_level();

    #if defined(PBL_HEALTH)
    set_current_steps();
    update_current_steps();
    #endif

    update_week(get_current_week_number(tick_time));
    update_time(tick_time);
    update_week_day(tick_time);
    update_date(tick_time);
}