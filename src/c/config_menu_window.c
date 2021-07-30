#include "config_menu_window.h"

#include <pebble.h>

#include "config_menu_window_logic.h"

static Window *config_window;

static StatusBarLayer* status_bar;

static SimpleMenuLayer* alarms_menu_layer;

static SimpleMenuItem m_alarm_items[5];
static SimpleMenuItem m_reset_items[1];
static SimpleMenuSection m_alarms_section;
static SimpleMenuSection m_reset_sections;
static SimpleMenuSection m_menu[2];

static void update_config_menu(Window* config_window)
{
    update_config_menu_items(m_alarm_items);
    layer_mark_dirty(simple_menu_layer_get_layer(alarms_menu_layer));
}

static void setup_alarms_menu_layer(Layer *window_layer, GRect bounds)
{
    m_alarms_section.num_items = 5;
    m_alarms_section.title = "Alarms";
    m_alarms_section.items = m_alarm_items;

    m_reset_items[0].title = "Reset";
    m_reset_items[0].callback = reset_alarms;

    m_reset_sections.num_items = 1;
    m_reset_sections.title = "Reset";
    m_reset_sections.items = m_reset_items;

    m_menu[0] = m_alarms_section;
    m_menu[1] = m_reset_sections;
    alarms_menu_layer = simple_menu_layer_create(
        GRect(0, STATUS_BAR_LAYER_HEIGHT, bounds.size.w, bounds.size.h - STATUS_BAR_LAYER_HEIGHT),
        config_window,
        m_menu, 2, NULL);

    layer_add_child(window_layer, simple_menu_layer_get_layer(alarms_menu_layer));
}

static void setup_status_bar(Layer *window_layer, GRect bounds)
{
    status_bar = status_bar_layer_create();

    status_bar_layer_set_colors(status_bar, GColorBlack, GColorWhite);
    status_bar_layer_set_separator_mode(status_bar, StatusBarLayerSeparatorModeDotted);

    layer_add_child(window_layer, status_bar_layer_get_layer(status_bar));
}

static void load_config_menu_window(Window *config_window)
{
    window_set_background_color(config_window, GColorBlack);
    Layer *config_window_layer = window_get_root_layer(config_window);
    GRect config_window_bounds = layer_get_bounds(config_window_layer);

    setup_alarms_menu_layer(config_window_layer, config_window_bounds);
    setup_status_bar(config_window_layer, config_window_bounds);
}

static void unload_config_menu_window(Window *window)
{
    simple_menu_layer_destroy(alarms_menu_layer);
    status_bar_layer_destroy(status_bar);
}

void setup_config_menu_window()
{
    config_window = window_create();

    window_set_window_handlers(config_window, (WindowHandlers) {
        .load = load_config_menu_window,
        .unload = unload_config_menu_window,
        .appear = update_config_menu
    });

    window_stack_push(config_window, true);
}

void tear_down_config_menu_window()
{
    window_destroy(config_window);
}