#include <pebble.h>
#include "setup.h"
#include "persistance.h"

static Window *main_window;
static Window *config_window;
static Window *edit_alarm_window;

static Window *alarm_window;

static TextLayer *title_layer;
static TextLayer *next_alarm_label_layer;
static TextLayer *next_alarm_layer;
static TextLayer *edit_alarm_active_layer;
static TextLayer *edit_alarm_time_hour_layer;
static TextLayer *edit_alarm_time_colon_layer;
static TextLayer *edit_alarm_time_minute_layer;

static TextLayer *alarm_title_layer;

static ActionBarLayer* main_window_action_bar_layer;
static ActionBarLayer* edit_alarm_action_bar_layer;

static ActionBarLayer* alarm_window_action_bar_layer;

static SimpleMenuLayer* alarms_menu_layer;

static GBitmap *check_icon;
static GBitmap *config_icon;
static GBitmap *alarm_icon;
static GBitmap *no_alarm_icon;
static GBitmap *edit_icon;
static GBitmap *up_icon;
static GBitmap *down_icon;
static GBitmap *play_icon;

static char m_titles[5][6];
static AlarmTimeOfDay* m_edit_alarm;
static bool m_icons_loaded = false;
static SimpleMenuItem m_menu_items[5];
static SimpleMenuSection m_menu;

static const char* singleInt = "0%d";
static const char* doubleInt = "%d";

static char* single_hour_single_minute = "0%d:0%d";
static char* single_hour_double_minute = "0%d:%d";
static char* double_hour_double_minute = "%d:%d";
static char* double_hour_single_minute = "%d:0%d";

static void take_next_medicine(ClickRecognizerRef recognizer, void* context)
{
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Take next medicine requested");
}

static void ensure_icons_loaded()
{
    if(!m_icons_loaded)
    {
        check_icon = gbitmap_create_with_resource(RESOURCE_ID_CHECK_ICON);
        config_icon = gbitmap_create_with_resource(RESOURCE_ID_CONFIG_ICON);
        alarm_icon = gbitmap_create_with_resource(RESOURCE_ID_ALARM_ICON);
        no_alarm_icon = gbitmap_create_with_resource(RESOURCE_ID_NO_ALARM_ICON);
        edit_icon = gbitmap_create_with_resource(RESOURCE_ID_EDIT_ICON);
        up_icon = gbitmap_create_with_resource(RESOURCE_ID_UP_ICON);
        down_icon = gbitmap_create_with_resource(RESOURCE_ID_DOWN_ICON);
        play_icon = gbitmap_create_with_resource(RESOURCE_ID_PLAY_ICON);
        m_icons_loaded = true;
    }
}


static void highlight_hours()
{
    text_layer_set_background_color(edit_alarm_time_hour_layer, GColorWhite);
    text_layer_set_text_color(edit_alarm_time_hour_layer, GColorBlack);
}

static void un_highlight_hours()
{
    text_layer_set_background_color(edit_alarm_time_hour_layer, GColorBlack);
    text_layer_set_text_color(edit_alarm_time_hour_layer, GColorWhite);
}

static void highlight_minutes()
{
    text_layer_set_background_color(edit_alarm_time_minute_layer, GColorWhite);
    text_layer_set_text_color(edit_alarm_time_minute_layer, GColorBlack);
}

static void un_highlight_minutes()
{
    text_layer_set_background_color(edit_alarm_time_minute_layer, GColorBlack);
    text_layer_set_text_color(edit_alarm_time_minute_layer, GColorWhite);
}

static void toggle_enabled(ClickRecognizerRef recognizer, void* context)
{
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Toggle Enabled requested");
    m_edit_alarm->active = !m_edit_alarm->active;
    text_layer_set_text(edit_alarm_active_layer, m_edit_alarm->active ? "Active" : "Inactive");
    save_data();
}

static void increase_time(ClickRecognizerRef recognizer, void* context);
static void goto_next_edit(ClickRecognizerRef recognizer, void* context);
static void goto_previous_edit(ClickRecognizerRef recognizer, void* context);
static void decrease_time(ClickRecognizerRef recognizer, void* context);
static void save_alarm_and_go_back(ClickRecognizerRef recognizer, void* context);
static void update_edit_alarm_time_layers(Window* window);
static short m_current_edit_step = 0;

static void edit_time_action_bar_click_config_provider(void* context)
{
    window_single_repeating_click_subscribe(BUTTON_ID_UP, 100, increase_time);
    window_single_click_subscribe(BUTTON_ID_SELECT, goto_next_edit);
    window_single_click_subscribe(BUTTON_ID_BACK, goto_previous_edit);
    window_single_repeating_click_subscribe(BUTTON_ID_DOWN, 100, decrease_time);
}

static void edit_time(ClickRecognizerRef recognizer, void* context)
{
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Edit time requested");
    action_bar_layer_set_click_config_provider(edit_alarm_action_bar_layer, edit_time_action_bar_click_config_provider);
    action_bar_layer_set_icon_animated(edit_alarm_action_bar_layer, BUTTON_ID_UP, up_icon, true);
    action_bar_layer_set_icon_animated(edit_alarm_action_bar_layer, BUTTON_ID_SELECT, play_icon, true);
    action_bar_layer_set_icon_animated(edit_alarm_action_bar_layer, BUTTON_ID_DOWN, down_icon, true);
    m_current_edit_step = 0;

    highlight_hours();
}

static void edit_alarm_action_bar_click_config_provider(void* context)
{
    window_single_click_subscribe(BUTTON_ID_UP, toggle_enabled);
    window_single_click_subscribe(BUTTON_ID_SELECT, edit_time);
    window_single_click_subscribe(BUTTON_ID_DOWN, save_alarm_and_go_back);
}

static void change_to_init_edit_alarm_actions()
{
    action_bar_layer_set_click_config_provider(edit_alarm_action_bar_layer, edit_alarm_action_bar_click_config_provider);

    ensure_icons_loaded();
    action_bar_layer_set_icon_animated(edit_alarm_action_bar_layer, BUTTON_ID_UP, m_edit_alarm->active ? alarm_icon : no_alarm_icon, true);
    action_bar_layer_set_icon_animated(edit_alarm_action_bar_layer, BUTTON_ID_SELECT, edit_icon, true);
    action_bar_layer_set_icon_animated(edit_alarm_action_bar_layer, BUTTON_ID_DOWN, check_icon, true);
}

static void increase_time(ClickRecognizerRef recognizer, void* context)
{
    if(m_current_edit_step == 0)
    {
        if(m_edit_alarm->hour < 23)
        {
            m_edit_alarm->hour++;
        } else
        {
            m_edit_alarm->hour = 0;
        }

    } else if(m_current_edit_step == 1)
    {
        if(m_edit_alarm->minute < 59)
        {
            m_edit_alarm->minute++;
        } else
        {
            m_edit_alarm->minute = 0;
        }
    }
    update_edit_alarm_time_layers(NULL);
}

static void decrease_time(ClickRecognizerRef recognizer, void* context)
{
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Decrease time requested");
    if(m_current_edit_step == 0)
    {
        if(m_edit_alarm->hour > 0)
        {
            m_edit_alarm->hour--;
        } else
        {
            m_edit_alarm->hour = 23;
        }
    } else if(m_current_edit_step == 1)
    {
        if(m_edit_alarm->minute > 0)
        {
            m_edit_alarm->minute--;
        } else
        {
            m_edit_alarm->minute = 59;
        }
    }
    update_edit_alarm_time_layers(NULL);
}

static void goto_next_edit(ClickRecognizerRef recognizer, void* context)
{
    APP_LOG(APP_LOG_LEVEL_DEBUG, "goto next edit requested");
    m_current_edit_step++;
    if(m_current_edit_step > 1)
    {
        change_to_init_edit_alarm_actions();
        un_highlight_minutes();
    } else if(m_current_edit_step == 1)
    {
        un_highlight_hours();
        highlight_minutes();
    }
}

static void goto_previous_edit(ClickRecognizerRef recognizer, void* context)
{
    m_current_edit_step--;
    if(m_current_edit_step < 0)
    {
        change_to_init_edit_alarm_actions();
        un_highlight_hours();
    } else if(m_current_edit_step == 0)
    {
        highlight_hours();
        un_highlight_minutes();
    }
}

static void save_alarm_and_go_back(ClickRecognizerRef recognizer, void* context)
{
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Save Alarm requested");
    save_data();
    window_stack_pop(true);
}

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

    text_layer_set_text(edit_alarm_active_layer, m_edit_alarm->active ? "Active" : "Inactive");

    layer_add_child(window_layer, text_layer_get_layer(edit_alarm_active_layer));
}

static void update_edit_alarm_time_layers(Window* window)
{
    static char hour_buffer[3];
    static char minute_buffer[3];

    if(m_edit_alarm->hour < 10)
    {
        snprintf(hour_buffer, sizeof(hour_buffer), singleInt, m_edit_alarm->hour);
    } else
    {
        snprintf(hour_buffer, sizeof(hour_buffer), doubleInt, m_edit_alarm->hour);
    }

    if(m_edit_alarm->minute < 10)
    {
        snprintf(minute_buffer, sizeof(minute_buffer), singleInt, m_edit_alarm->minute);
    } else
    {
        snprintf(minute_buffer, sizeof(minute_buffer), doubleInt, m_edit_alarm->minute);
    }

    text_layer_set_text(edit_alarm_time_hour_layer, hour_buffer);
    text_layer_set_text(edit_alarm_time_minute_layer, minute_buffer);
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

static void handle_alarm_edit(int index, void* context)
{
    m_edit_alarm = GetAlarm(index);

    edit_alarm_window = window_create();

    window_set_window_handlers(edit_alarm_window, (WindowHandlers) {
        .load = setup_edit_alarm_window,
        .unload = tear_down_edit_alarm_window,
        .appear = update_edit_alarm_time_layers
    });

    window_stack_push(edit_alarm_window, true);
}

static char* get_format_for_alarm(AlarmTimeOfDay* current)
{
    APP_LOG(APP_LOG_LEVEL_DEBUG, "get_format_for_alarm");
    char* format = NULL;
    if(current->hour > 9)
    {
        if(current->minute > 9)
        {
            format = double_hour_double_minute;
        } else
        {
            format = double_hour_single_minute;
        }
    } else
    {
        if(current->minute > 9)
        {
            format = single_hour_double_minute;
        } else
        {
            format = single_hour_single_minute;
        }
    }
    APP_LOG(APP_LOG_LEVEL_DEBUG, "get_format_for_alarm done");
    return format;
}

static void set_menu_item_title(int index, AlarmTimeOfDay* current)
{
    snprintf(m_titles[index], sizeof(m_titles[index]), get_format_for_alarm(current), current->hour, current->minute);
}

static void update_config_menu_items(Window* config_window)
{
    ensure_icons_loaded();
    AlarmTimeOfDay* alarms = GetAlarms();
    for(int i = 0; i < MAX_ALARMS; i++)
    {
        AlarmTimeOfDay* current = (alarms + i);
        m_menu_items[i].icon = current->active ? alarm_icon : no_alarm_icon;
        m_menu_items[i].callback = handle_alarm_edit;
        set_menu_item_title(i, current);

        m_menu_items[i].title = m_titles[i];
        m_menu_items[i].subtitle = current->active ? "Active" : "Not Active";
    }
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

static void tear_down_config_window(Window *window)
{
    simple_menu_layer_destroy(alarms_menu_layer);
}

static void goto_config_window(ClickRecognizerRef recognizer, void* context)
{
    config_window = window_create();

    window_set_window_handlers(config_window, (WindowHandlers) {
        .load = setup_config_window,
        .unload = tear_down_config_window,
        .appear = update_config_menu_items
    });

    window_stack_push(config_window, true);
}

static void main_window_click_config_provider(void* context)
{
    window_single_click_subscribe(BUTTON_ID_DOWN, goto_config_window);
    window_long_click_subscribe(BUTTON_ID_SELECT, 500, take_next_medicine, NULL);
}

static void silence_alarm(ClickRecognizerRef recognizer, void* context)
{
    APP_LOG(APP_LOG_LEVEL_DEBUG, "silence alarm requested");
}

static void take_medicine(ClickRecognizerRef recognizer, void* context)
{
    APP_LOG(APP_LOG_LEVEL_DEBUG, "take medicine requested");
}

static void snooze_alarm(ClickRecognizerRef recognizer, void* context)
{
    APP_LOG(APP_LOG_LEVEL_DEBUG, "snooze alarm requested");
}

static void handle_back_alarm(ClickRecognizerRef recognizer, void* context)
{
    APP_LOG(APP_LOG_LEVEL_DEBUG, "back pressed on alarm");
}

static void alarm_window_click_config_provider(void* context)
{
    window_single_click_subscribe(BUTTON_ID_UP, silence_alarm);
    window_single_click_subscribe(BUTTON_ID_BACK, handle_back_alarm);
    window_long_click_subscribe(BUTTON_ID_SELECT, 500, take_medicine, NULL);
    window_single_repeating_click_subscribe(BUTTON_ID_DOWN, 500, snooze_alarm);
}

static void setup_main_window_action_bar_layer(Layer *window_layer, GRect bounds)
{
    APP_LOG(APP_LOG_LEVEL_DEBUG, "setup_main_window_action_bar_layer");
    main_window_action_bar_layer = action_bar_layer_create();
    action_bar_layer_set_background_color(main_window_action_bar_layer, GColorWhite);
    action_bar_layer_add_to_window(main_window_action_bar_layer, main_window);
    action_bar_layer_set_click_config_provider(main_window_action_bar_layer, main_window_click_config_provider);

    ensure_icons_loaded();

    action_bar_layer_set_icon_animated(main_window_action_bar_layer, BUTTON_ID_SELECT, check_icon, true);
    action_bar_layer_set_icon_animated(main_window_action_bar_layer, BUTTON_ID_DOWN, config_icon, true);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "setup_main_window_action_bar_layer done");
}

static void setup_alarm_window_action_bar_layer(Layer *window_layer, GRect bounds)
{
    alarm_window_action_bar_layer = action_bar_layer_create();
    action_bar_layer_set_background_color(alarm_window_action_bar_layer, GColorWhite);
    action_bar_layer_add_to_window(alarm_window_action_bar_layer, alarm_window);
    action_bar_layer_set_click_config_provider(alarm_window_action_bar_layer, alarm_window_click_config_provider);

    ensure_icons_loaded();

    action_bar_layer_set_icon_animated(alarm_window_action_bar_layer, BUTTON_ID_UP, alarm_icon, true);
    action_bar_layer_set_icon_animated(alarm_window_action_bar_layer, BUTTON_ID_SELECT, check_icon, true);
    action_bar_layer_set_icon_animated(alarm_window_action_bar_layer, BUTTON_ID_DOWN, no_alarm_icon, true);
}

static void update_next_alarm_text(Window *window)
{
    APP_LOG(APP_LOG_LEVEL_DEBUG, "update_next_alarm_text");
    AlarmTimeOfDay* next = GetNextAlarmTime();
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Got next alarm");
    static char next_alarm_buffer[6];
    if(next == NULL)
    {
        snprintf(next_alarm_buffer, sizeof(next_alarm_buffer), "None");
    } else
    {
        snprintf(next_alarm_buffer, sizeof(next_alarm_buffer), get_format_for_alarm(next), next->hour, next->minute);
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Got format");
    }
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Setting text");
    text_layer_set_text(next_alarm_layer, next_alarm_buffer);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "update_next_alarm_text done");
}

static void setup_next_alarm_layer(Layer *window_layer, GRect bounds)
{
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Setting up next alarm layer");
    next_alarm_label_layer = text_layer_create(GRect(0, 70, bounds.size.w - 10, 30));
    next_alarm_layer = text_layer_create(GRect(0, 94, bounds.size.w - 10, 30));

    text_layer_set_background_color(next_alarm_label_layer, GColorBlack);
    text_layer_set_background_color(next_alarm_layer, GColorBlack);
    text_layer_set_text_color(next_alarm_label_layer, GColorWhite);
    text_layer_set_text_color(next_alarm_layer, GColorWhite);
    text_layer_set_font(next_alarm_label_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
    text_layer_set_font(next_alarm_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
    text_layer_set_text_alignment(next_alarm_label_layer, GTextAlignmentLeft);
    text_layer_set_text_alignment(next_alarm_layer, GTextAlignmentLeft);

    static char next_buffer[12];
    snprintf(next_buffer, sizeof(next_buffer), "Next Alarm:");
    text_layer_set_text(next_alarm_label_layer, next_buffer);

    layer_add_child(window_layer, text_layer_get_layer(next_alarm_label_layer));
    layer_add_child(window_layer, text_layer_get_layer(next_alarm_layer));
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Setting up next alarm layer done");
}

static void setup_title_layer(Layer *window_layer, GRect bounds)
{
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Setting up title layer");
    title_layer = text_layer_create(GRect(0, 0, bounds.size.w - ACTION_BAR_WIDTH, 60));

    text_layer_set_background_color(title_layer, GColorBlack);
    text_layer_set_text_color(title_layer, GColorWhite);
    text_layer_set_font(title_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
    text_layer_set_text_alignment(title_layer, GTextAlignmentCenter);

    static char title_buffer[18];
    snprintf(title_buffer, sizeof(title_buffer), "Meds Reminder");
    text_layer_set_text(title_layer, title_buffer);

    layer_add_child(window_layer, text_layer_get_layer(title_layer));
}

static void setup_alarm_title_layer(Layer *window_layer, GRect bounds)
{
    alarm_title_layer = text_layer_create(GRect(0, 0, bounds.size.w - ACTION_BAR_WIDTH, 60));

    text_layer_set_background_color(alarm_title_layer, GColorBlack);
    text_layer_set_text_color(alarm_title_layer, GColorWhite);
    text_layer_set_font(alarm_title_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
    text_layer_set_text_alignment(alarm_title_layer, GTextAlignmentCenter);

    static char alarm_title_buffer[18];
    snprintf(alarm_title_buffer, sizeof(alarm_title_buffer), "Take Your Meds");
    text_layer_set_text(alarm_title_layer, alarm_title_buffer);

    layer_add_child(window_layer, text_layer_get_layer(alarm_title_layer));
}

static void setup_main_window(Window *window)
{
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Setting up main window");
    window_set_background_color(window, GColorBlack);
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    setup_title_layer(window_layer, bounds);
    setup_next_alarm_layer(window_layer, bounds);
    setup_main_window_action_bar_layer(window_layer, bounds);
}

static void tear_down_main_window(Window *window)
{
    text_layer_destroy(title_layer);
    text_layer_destroy(next_alarm_layer);
    text_layer_destroy(next_alarm_label_layer);
    action_bar_layer_remove_from_window(main_window_action_bar_layer);
    action_bar_layer_destroy(main_window_action_bar_layer);
}

static void setup_alarm_window(Window *window)
{
    window_set_background_color(window, GColorBlack);
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    setup_alarm_title_layer(window_layer, bounds);
    setup_alarm_window_action_bar_layer(window_layer, bounds);
}

static void tear_down_alarm_window(Window *window)
{
    text_layer_destroy(alarm_title_layer);
    // text_layer_destroy(next_alarm_layer);
    // text_layer_destroy(next_alarm_label_layer);
    // action_bar_layer_remove_from_window(main_window_action_bar_layer);
    // action_bar_layer_destroy(main_window_action_bar_layer);
}

void setup_alarm()
{
    alarm_window = window_create();

       window_set_window_handlers(alarm_window, (WindowHandlers) {
        .load = setup_alarm_window,
        .unload = tear_down_alarm_window
    });

    window_stack_push(alarm_window, true);
}

void setup_app()
{
    main_window = window_create();

    window_set_window_handlers(main_window, (WindowHandlers) {
        .load = setup_main_window,
        .unload = tear_down_main_window,
        .appear = update_next_alarm_text
    });

    window_stack_push(main_window, true);
}

void tear_down_app()
{
    window_destroy(main_window);
}