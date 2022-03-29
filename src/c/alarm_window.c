#include "alarm_window.h"

#include <pebble.h>

#include "alarm_window_logic.h"
#include "persistance.h"
#include "icons.h"

static Window *alarm_window;

static StatusBarLayer* status_bar;

static TextLayer *alarm_title_layer;
static TextLayer *alarm_time_layer;
static TextLayer *snooze_time_layer;

static ActionBarLayer* alarm_window_action_bar_layer;

static GColor8 m_background_color;
static GColor8 m_foreground_color;

static void setup_alarm_window_action_bar_layer(Layer *window_layer, GRect bounds)
{
    alarm_window_action_bar_layer = action_bar_layer_create();
    action_bar_layer_set_background_color(alarm_window_action_bar_layer, m_foreground_color);
    action_bar_layer_add_to_window(alarm_window_action_bar_layer, alarm_window);
    action_bar_layer_set_click_config_provider(alarm_window_action_bar_layer, alarm_window_click_config_provider);

    action_bar_layer_set_icon_animated(alarm_window_action_bar_layer, BUTTON_ID_UP, get_silence_icon(), true);
    action_bar_layer_set_icon_animated(alarm_window_action_bar_layer, BUTTON_ID_SELECT, get_check_icon(), true);
    action_bar_layer_set_icon_animated(alarm_window_action_bar_layer, BUTTON_ID_DOWN, get_snooze_icon(), true);
}

static void setup_alarm_title_layer(Layer *window_layer, GRect bounds)
{
    alarm_title_layer = text_layer_create(GRect(0, STATUS_BAR_LAYER_HEIGHT, bounds.size.w - ACTION_BAR_WIDTH, 60));

    text_layer_set_background_color(alarm_title_layer, m_background_color);
    text_layer_set_text_color(alarm_title_layer, m_foreground_color);
    text_layer_set_font(alarm_title_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
    text_layer_set_text_alignment(alarm_title_layer, GTextAlignmentCenter);

    text_layer_set_text(alarm_title_layer, "Take Your Meds");

    layer_add_child(window_layer, text_layer_get_layer(alarm_title_layer));
}

static void setup_alarm_time_layer(Layer *window_layer, GRect bounds)
{
    alarm_time_layer = text_layer_create(GRect(0, 72, bounds.size.w - ACTION_BAR_WIDTH, 60));

    text_layer_set_background_color(alarm_time_layer, m_background_color);
    text_layer_set_text_color(alarm_time_layer, m_foreground_color);
    text_layer_set_font(alarm_time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
    text_layer_set_text_alignment(alarm_time_layer, GTextAlignmentCenter);

    text_layer_set_text(alarm_time_layer, get_wakeup_alarm_time_string());

    layer_add_child(window_layer, text_layer_get_layer(alarm_time_layer));
}

static void setup_alarm_snooze_time_layer(Layer *window_layer, GRect bounds)
{
    uint16_t width = bounds.size.w - ACTION_BAR_WIDTH;
    snooze_time_layer = text_layer_create(GRect(width - 60, bounds.size.h - 50, 50, 30));

    text_layer_set_background_color(snooze_time_layer, m_background_color);
    text_layer_set_text_color(snooze_time_layer, m_foreground_color);
    text_layer_set_font(snooze_time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
    text_layer_set_text_alignment(snooze_time_layer, GTextAlignmentRight);

    text_layer_set_text(snooze_time_layer, "5m");

    layer_add_child(window_layer, text_layer_get_layer(snooze_time_layer));
}

static void setup_status_bar(Layer *window_layer, GRect bounds)
{
    status_bar = status_bar_layer_create();

    status_bar_layer_set_colors(status_bar, m_background_color, m_foreground_color);
    status_bar_layer_set_separator_mode(status_bar, StatusBarLayerSeparatorModeDotted);

    layer_add_child(window_layer, status_bar_layer_get_layer(status_bar));
}

static void load_alarm_window(Window *window)
{
    window_set_background_color(window, m_background_color);
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    setup_alarm_title_layer(window_layer, bounds);
    setup_alarm_time_layer(window_layer, bounds);
    setup_alarm_snooze_time_layer(window_layer, bounds);
    setup_alarm_window_action_bar_layer(window_layer, bounds);
    setup_status_bar(window_layer, bounds);

    set_alarm_layers(alarm_time_layer, snooze_time_layer, alarm_window_action_bar_layer);
}

static void unload_alarm_window(Window *window)
{
    text_layer_destroy(alarm_title_layer);
    text_layer_destroy(alarm_time_layer);
    text_layer_destroy(snooze_time_layer);
    action_bar_layer_remove_from_window(alarm_window_action_bar_layer);
    action_bar_layer_destroy(alarm_window_action_bar_layer);
    status_bar_layer_destroy(status_bar);
}

void setup_alarm_window(int32_t alarm_index, GColor8 background_color, GColor8 foreground_color)
{
    m_background_color = background_color;
    m_foreground_color = foreground_color;

    setup_alarm_state(alarm_index);
    alarm_window = window_create();
    set_alarm_window(alarm_window);
    window_set_window_handlers(alarm_window, (WindowHandlers) {
        .load = load_alarm_window,
        .unload = unload_alarm_window
    });

    window_stack_push(alarm_window, true);
}

void tear_down_alarm_window()
{
    window_destroy(alarm_window);
}