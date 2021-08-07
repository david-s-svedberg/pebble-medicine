#include "edit_alarm_window_logic.h"

#include <pebble.h>

#include "persistance.h"
#include "format.h"
#include "icons.h"
#include "scheduler.h"

static TextLayer *m_edit_alarm_active_layer;
static TextLayer *m_edit_alarm_time_hour_layer;
static TextLayer *m_edit_alarm_time_minute_layer;

static ActionBarLayer* m_edit_alarm_action_bar_layer;

Alarm* m_edit_alarm;
static short m_current_edit_step = 0;

static void save_alarm_and_go_back(ClickRecognizerRef recognizer, void* context)
{
    if(m_edit_alarm->active)
    {
        reschedule_alarm(m_edit_alarm);
    } else
    {
        unschedule_alarm(m_edit_alarm);
    }
    save_data();
    window_stack_pop(true);
}

static void highlight_hours()
{
    text_layer_set_background_color(m_edit_alarm_time_hour_layer, GColorWhite);
    text_layer_set_text_color(m_edit_alarm_time_hour_layer, GColorBlack);
}

static void un_highlight_hours()
{
    text_layer_set_background_color(m_edit_alarm_time_hour_layer, GColorBlack);
    text_layer_set_text_color(m_edit_alarm_time_hour_layer, GColorWhite);
}

static void highlight_minutes()
{
    text_layer_set_background_color(m_edit_alarm_time_minute_layer, GColorWhite);
    text_layer_set_text_color(m_edit_alarm_time_minute_layer, GColorBlack);
}

static void un_highlight_minutes()
{
    text_layer_set_background_color(m_edit_alarm_time_minute_layer, GColorBlack);
    text_layer_set_text_color(m_edit_alarm_time_minute_layer, GColorWhite);
}

static void update_alarm_active_text()
{
    text_layer_set_text(m_edit_alarm_active_layer, m_edit_alarm->active ? "Active" : "Inactive");
}

static void toggle_enabled(ClickRecognizerRef recognizer, void* context)
{
    m_edit_alarm->active = !m_edit_alarm->active;
    update_alarm_active_text();
}

static void increase_time(ClickRecognizerRef recognizer, void* context)
{
    if(m_current_edit_step == 0)
    {
        if(m_edit_alarm->time.hour < 23)
        {
            m_edit_alarm->time.hour++;
        } else
        {
            m_edit_alarm->time.hour = 0;
        }

    } else if(m_current_edit_step == 1)
    {
        if(m_edit_alarm->time.minute < 59)
        {
            m_edit_alarm->time.minute++;
        } else
        {
            m_edit_alarm->time.minute = 0;
        }
    }
    update_edit_alarm_time_layers();
}

static void decrease_time(ClickRecognizerRef recognizer, void* context)
{
    if(m_current_edit_step == 0)
    {
        if(m_edit_alarm->time.hour > 0)
        {
            m_edit_alarm->time.hour--;
        } else
        {
            m_edit_alarm->time.hour = 23;
        }
    } else if(m_current_edit_step == 1)
    {
        if(m_edit_alarm->time.minute > 0)
        {
            m_edit_alarm->time.minute--;
        } else
        {
            m_edit_alarm->time.minute = 59;
        }
    }
    update_edit_alarm_time_layers();
}

static void goto_next_edit(ClickRecognizerRef recognizer, void* context)
{
    m_current_edit_step++;
    if(m_current_edit_step > 1)
    {
        change_to_init_edit_alarm_actions();
        m_edit_alarm->active = true;
        update_alarm_active_text();
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

static void edit_time_action_bar_click_config_provider(void* context)
{
    window_single_repeating_click_subscribe(BUTTON_ID_UP, 100, increase_time);
    window_single_click_subscribe(BUTTON_ID_SELECT, goto_next_edit);
    window_single_click_subscribe(BUTTON_ID_BACK, goto_previous_edit);
    window_single_repeating_click_subscribe(BUTTON_ID_DOWN, 100, decrease_time);
}

static void edit_time(ClickRecognizerRef recognizer, void* context)
{
    action_bar_layer_set_click_config_provider(m_edit_alarm_action_bar_layer, edit_time_action_bar_click_config_provider);
    action_bar_layer_set_icon_animated(m_edit_alarm_action_bar_layer, BUTTON_ID_UP, get_up_icon(), true);
    action_bar_layer_set_icon_animated(m_edit_alarm_action_bar_layer, BUTTON_ID_SELECT, get_play_icon(), true);
    action_bar_layer_set_icon_animated(m_edit_alarm_action_bar_layer, BUTTON_ID_DOWN, get_down_icon(), true);
    m_current_edit_step = 0;

    highlight_hours();
}

void change_to_init_edit_alarm_actions()
{
    action_bar_layer_set_click_config_provider(m_edit_alarm_action_bar_layer, edit_alarm_action_bar_click_config_provider);

    action_bar_layer_set_icon_animated(m_edit_alarm_action_bar_layer, BUTTON_ID_UP, m_edit_alarm->active ? get_alarm_icon() : get_no_alarm_icon(), true);
    action_bar_layer_set_icon_animated(m_edit_alarm_action_bar_layer, BUTTON_ID_SELECT, get_edit_icon(), true);
    action_bar_layer_set_icon_animated(m_edit_alarm_action_bar_layer, BUTTON_ID_DOWN, get_check_icon(), true);
}


void edit_alarm_action_bar_click_config_provider(void* context)
{
    window_single_click_subscribe(BUTTON_ID_UP, toggle_enabled);
    window_single_click_subscribe(BUTTON_ID_SELECT, edit_time);
    window_single_click_subscribe(BUTTON_ID_DOWN, save_alarm_and_go_back);
}

void update_edit_alarm_time_layers()
{
    static char hour_buffer[3];
    static char minute_buffer[3];
    fill_time_unit_string(hour_buffer, m_edit_alarm->time.hour);
    fill_time_unit_string(minute_buffer, m_edit_alarm->time.minute);

    text_layer_set_text(m_edit_alarm_time_hour_layer, hour_buffer);
    text_layer_set_text(m_edit_alarm_time_minute_layer, minute_buffer);
}

void set_edit_alarm_layers(
    TextLayer *edit_alarm_active_layer,
    TextLayer *edit_alarm_time_hour_layer,
    TextLayer *edit_alarm_time_minute_layer,
    ActionBarLayer* edit_alarm_action_bar_layer
)
{
    m_edit_alarm_active_layer = edit_alarm_active_layer;
    m_edit_alarm_time_hour_layer = edit_alarm_time_hour_layer;
    m_edit_alarm_time_minute_layer = edit_alarm_time_minute_layer;
    m_edit_alarm_action_bar_layer = edit_alarm_action_bar_layer;
}

void set_edit_alarm(int edit_alarm_index)
{
    m_edit_alarm = get_alarm(edit_alarm_index);
}

Alarm* get_edit_alarm()
{
    return m_edit_alarm;
}