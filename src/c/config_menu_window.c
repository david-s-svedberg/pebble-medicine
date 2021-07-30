#include "config_menu_window.h"

#include <pebble.h>

#include "config_menu_window_logic.h"

static Window *config_window;

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
    alarms_menu_layer = simple_menu_layer_create(bounds, config_window, m_menu, 2, NULL);

    layer_add_child(window_layer, simple_menu_layer_get_layer(alarms_menu_layer));
}

static void load_config_menu_window(Window *config_window)
{
    window_set_background_color(config_window, GColorBlack);
    Layer *config_window_layer = window_get_root_layer(config_window);
    GRect config_window_bounds = layer_get_bounds(config_window_layer);

    setup_alarms_menu_layer(config_window_layer, config_window_bounds);
}

static void unload_config_menu_window(Window *window)
{
    simple_menu_layer_destroy(alarms_menu_layer);
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