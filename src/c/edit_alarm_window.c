#include "edit_alarm_window.h"

#include <pebble.h>

#include "edit_alarm_window_logic.h"
#include "persistance.h"
#include "icons.h"

static Window *edit_alarm_window;

static TextLayer *edit_alarm_active_layer;
static TextLayer *edit_alarm_time_hour_layer;
static TextLayer *edit_alarm_time_colon_layer;
static TextLayer *edit_alarm_time_minute_layer;

static ActionBarLayer* edit_alarm_action_bar_layer;

static void setup_edit_alarm_action_bar_layer(Layer *window_layer, GRect bounds)
{
    edit_alarm_action_bar_layer = action_bar_layer_create();
    action_bar_layer_set_background_color(edit_alarm_action_bar_layer, GColorWhite);
    action_bar_layer_add_to_window(edit_alarm_action_bar_layer, edit_alarm_window);
    action_bar_layer_set_click_config_provider(edit_alarm_action_bar_layer, edit_alarm_action_bar_click_config_provider);

    change_to_init_edit_alarm_actions();
}

static void setup_edit_alarm_active_layer(Layer *window_layer, GRect bounds)
{
    edit_alarm_active_layer = text_layer_create(GRect(0, 14, bounds.size.w - ACTION_BAR_WIDTH, 30));

    text_layer_set_background_color(edit_alarm_active_layer, GColorBlack);

    text_layer_set_text_color(edit_alarm_active_layer, GColorWhite);
    text_layer_set_font(edit_alarm_active_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));

    text_layer_set_text_alignment(edit_alarm_active_layer, GTextAlignmentCenter);

    text_layer_set_text(edit_alarm_active_layer, get_edit_alarm()->active ? "Active" : "Inactive");

    layer_add_child(window_layer, text_layer_get_layer(edit_alarm_active_layer));
}

static void update_edit_alarm_window(Window* window)
{
    update_edit_alarm_time_layers();
}

static void setup_edit_alarm_time_layer(Layer *window_layer, GRect bounds)
{
    uint16_t y = 68;
    uint16_t height = 37;
    edit_alarm_time_hour_layer = text_layer_create(GRect(20, y, 28, height));
    edit_alarm_time_colon_layer = text_layer_create(GRect(50, y - 1, 18, height));
    edit_alarm_time_minute_layer = text_layer_create(GRect(68, y, 28, height));

    text_layer_set_background_color(edit_alarm_time_hour_layer, GColorBlack);
    text_layer_set_background_color(edit_alarm_time_colon_layer, GColorBlack);
    text_layer_set_background_color(edit_alarm_time_minute_layer, GColorBlack);

    text_layer_set_text_color(edit_alarm_time_hour_layer, GColorWhite);
    text_layer_set_text_color(edit_alarm_time_colon_layer, GColorWhite);
    text_layer_set_text_color(edit_alarm_time_minute_layer, GColorWhite);

    text_layer_set_font(edit_alarm_time_hour_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
    text_layer_set_font(edit_alarm_time_colon_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
    text_layer_set_font(edit_alarm_time_minute_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));

    text_layer_set_text_alignment(edit_alarm_time_hour_layer, GTextAlignmentCenter);
    text_layer_set_text_alignment(edit_alarm_time_colon_layer, GTextAlignmentCenter);
    text_layer_set_text_alignment(edit_alarm_time_minute_layer, GTextAlignmentCenter);

    text_layer_set_text(edit_alarm_time_colon_layer, ":");

    layer_add_child(window_layer, text_layer_get_layer(edit_alarm_time_hour_layer));
    layer_add_child(window_layer, text_layer_get_layer(edit_alarm_time_colon_layer));
    layer_add_child(window_layer, text_layer_get_layer(edit_alarm_time_minute_layer));
}

static void setup_edit_alarm_window(Window *edit_alarm_window)
{
    window_set_background_color(edit_alarm_window, GColorBlack);
    Layer *edit_alarm_window_layer = window_get_root_layer(edit_alarm_window);
    GRect edit_alarm_window_bounds = layer_get_bounds(edit_alarm_window_layer);

    setup_edit_alarm_action_bar_layer(edit_alarm_window_layer, edit_alarm_window_bounds);
    setup_edit_alarm_active_layer(edit_alarm_window_layer, edit_alarm_window_bounds);
    setup_edit_alarm_time_layer(edit_alarm_window_layer, edit_alarm_window_bounds);
}

static void tear_down_edit_alarm_window(Window *window)
{
    text_layer_destroy(edit_alarm_active_layer);
    action_bar_layer_remove_from_window(edit_alarm_action_bar_layer);
    action_bar_layer_destroy(edit_alarm_action_bar_layer);
}

void setup_edit_alarm_window(AlarmTimeOfDay* edit_alarm)
{
    set_edit_alarm(edit_alarm);

    edit_alarm_window = window_create();

    window_set_window_handlers(edit_alarm_window, (WindowHandlers) {
        .load = setup_edit_alarm_window,
        .unload = tear_down_edit_alarm_window,
        .appear = update_edit_alarm_window
    });
    set_layers(
        edit_alarm_active_layer,
        edit_alarm_time_hour_layer,
        edit_alarm_time_minute_layer,
        edit_alarm_action_bar_layer);
    window_stack_push(edit_alarm_window, true);
}

void tear_down_edit_alarm_window()
{
    window_destroy(edit_alarm_window);
}