#include "alarm_window_logic.h"

#include <pebble.h>

#include "persistance.h"
#include "format.h"
#include "scheduler.h"

static Window *m_alarm_window;

static TextLayer *m_alarm_time_layer;
static TextLayer *m_snooze_time_layer;

static ActionBarLayer* m_alarm_window_action_bar_layer;

static Alarm* m_wakup_alarm;
static bool m_alarm_silenced = false;

static uint16_t snooze_progression[] = {5, 10, 15, 30, 60, 90, 120};
static uint16_t m_snooze_minutes = 5;
static AppTimer* m_snooze_selection_done = NULL;
static AppTimer* m_silenced_timedout = NULL;

static const uint32_t const segments[] = { 50 };
static const VibePattern m_vibration_pattern =
{
    .durations = segments,
    .num_segments = ARRAY_LENGTH(segments),
};

static const uint32_t FIVE_MINUTES_IN_MS = SECONDS_PER_MINUTE * 5 * 1000;

static void run_vibration(void* data)
{
    if(!m_alarm_silenced)
    {
        vibes_enqueue_custom_pattern(m_vibration_pattern);
        app_timer_register(2000, run_vibration, NULL);
    }
}

static void close_alarm()
{
    exit_reason_set(APP_EXIT_ACTION_PERFORMED_SUCCESSFULLY);
    window_stack_remove(m_alarm_window, true);
}

static void silence_timed_out(void* data)
{
    m_alarm_silenced = false;
    run_vibration(NULL);
}

static void silence_alarm(ClickRecognizerRef recognizer, void* context)
{
    APP_LOG(APP_LOG_LEVEL_DEBUG, "silence alarm requested");
    vibes_cancel();
    m_alarm_silenced = true;
    if(m_silenced_timedout != NULL)
    {
        app_timer_reschedule(m_silenced_timedout, FIVE_MINUTES_IN_MS);
    } else
    {
        m_silenced_timedout = app_timer_register(FIVE_MINUTES_IN_MS, silence_timed_out, NULL);
    }
}

static void take_medicine(ClickRecognizerRef recognizer, void* context)
{
    APP_LOG(APP_LOG_LEVEL_DEBUG, "take medicine requested");
    vibes_cancel();
    m_alarm_silenced = true;
    schedule_alarm(m_wakup_alarm);
    save_data();
    close_alarm();
}

static void snooze_selection_done(void* data)
{
    time_t t = time(NULL);
    t += (SECONDS_PER_MINUTE * m_snooze_minutes);
    schedule_snooze_alarm(m_wakup_alarm, t);
    save_data();
    close_alarm();
}

static uint8_t m_clicks = 0;

static void snooze_alarm(ClickRecognizerRef recognizer, void* context)
{
    APP_LOG(APP_LOG_LEVEL_DEBUG, "snooze alarm requested");

    m_clicks++;
    m_snooze_minutes = snooze_progression[(m_clicks - 1) % ARRAY_LENGTH(snooze_progression)];
    static char snooze_text_buffer[6];
    snprintf(snooze_text_buffer, sizeof(snooze_text_buffer), "%dm", m_snooze_minutes);
    text_layer_set_text(m_snooze_time_layer, snooze_text_buffer);
    m_alarm_silenced = true;
    if(m_snooze_selection_done != NULL)
    {
        app_timer_reschedule(m_snooze_selection_done, 1000);
    } else
    {
        m_snooze_selection_done = app_timer_register(1000, snooze_selection_done, NULL);
    }
}

static void handle_back_alarm(ClickRecognizerRef recognizer, void* context)
{
    APP_LOG(APP_LOG_LEVEL_DEBUG, "back pressed on alarm");
    time_t t = time(NULL);
    t += (SECONDS_PER_MINUTE * 5);
    wakeup_schedule(t, m_wakup_alarm->index, true);
    close_alarm();
}

void alarm_window_click_config_provider(void* context)
{
    window_single_click_subscribe(BUTTON_ID_UP, silence_alarm);
    window_single_click_subscribe(BUTTON_ID_BACK, handle_back_alarm);
    window_single_click_subscribe(BUTTON_ID_SELECT, take_medicine);
    window_single_repeating_click_subscribe(BUTTON_ID_DOWN, 500, snooze_alarm);
}

char* get_wakeup_alarm_time_string()
{
    static char alarm_time_buffer[6];
    fill_time_string(alarm_time_buffer, m_wakup_alarm->time.hour, m_wakup_alarm->time.minute);
    return alarm_time_buffer;
}

void setup_alarm_state(int32_t alarm_index)
{
    m_wakup_alarm = get_alarm(alarm_index);
    run_vibration(NULL);
}

void set_alarm_layers(
    TextLayer* alarm_time_layer,
    TextLayer* snooze_time_layer,
    ActionBarLayer* alarm_window_action_bar_layer)
{
    m_alarm_time_layer = alarm_time_layer;
    m_snooze_time_layer = snooze_time_layer;
    m_alarm_window_action_bar_layer = alarm_window_action_bar_layer;
}
void set_alarm_window(Window* alarm_window)
{
    m_alarm_window = alarm_window;
}