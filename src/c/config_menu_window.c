#include "config_menu_window.h"

#include <pebble.h>

#include "config_menu_window_logic.h"

static Window *config_window;

static SimpleMenuLayer* alarms_menu_layer;

static SimpleMenuItem m_menu_items[5];
static SimpleMenuSection m_menu;

static void update_config_menu(Window* config_window)
{
    update_config_menu_items(m_menu_items);
    layer_mark_dirty(simple_menu_layer_get_layer(alarms_menu_layer));
}

static void setup_alarms_menu_layer(Layer *window_layer, GRect bounds)
{
    m_menu.num_items = 5;
    m_menu.title = "Alarms";
    m_menu.items = m_menu_items;

    alarms_menu_layer = simple_menu_layer_create(bounds, config_window, &m_menu, 1, NULL);

    layer_add_child(window_layer, simple_menu_layer_get_layer(alarms_menu_layer));
}

static void setup_config_window(Window *config_window)
{
    window_set_background_color(config_window, GColorBlack);
    Layer *config_window_layer = window_get_root_layer(config_window);
    GRect config_window_bounds = layer_get_bounds(config_window_layer);

    setup_alarms_menu_layer(config_window_layer, config_window_bounds);
}

void tear_down_config_window(Window *window)
{
    simple_menu_layer_destroy(alarms_menu_layer);
}

void setup_config_menu_window()
{
    config_window = window_create();

    window_set_window_handlers(config_window, (WindowHandlers) {
        .load = setup_config_window,
        .unload = tear_down_config_window,
        .appear = update_config_menu
    });

    window_stack_push(config_window, true);
}

void tear_down_config_menu_window()
{
    window_destroy(config_window);
}