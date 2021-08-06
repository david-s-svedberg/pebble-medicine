#include "icons.h"

static GBitmap *check_icon;
static GBitmap *config_icon;
static GBitmap *alarm_icon;
static GBitmap *no_alarm_icon;
static GBitmap *alarm_icon_trans;
static GBitmap *no_alarm_icon_trans;
static GBitmap *edit_icon;
static GBitmap *up_icon;
static GBitmap *down_icon;
static GBitmap *play_icon;
static GBitmap *snooze_icon;
static GBitmap *silence_icon;

static GBitmap* get_icon(uint32_t id, GBitmap* icon)
{
    if(icon == NULL)
    {
        icon = gbitmap_create_with_resource(id);
    }

    return icon;
}

static void destroy_icon(GBitmap* icon)
{
    if(icon == NULL)
    {
        gbitmap_destroy(icon);
    }
}

GBitmap* get_check_icon()
{
    return get_icon(RESOURCE_ID_CHECK_ICON, check_icon);
}

GBitmap* get_config_icon()
{
    return get_icon(RESOURCE_ID_CONFIG_ICON, config_icon);
}

GBitmap* get_alarm_icon()
{
    return get_icon(RESOURCE_ID_ALARM_ICON, alarm_icon);
}

GBitmap* get_no_alarm_icon()
{
    return get_icon(RESOURCE_ID_NO_ALARM_ICON, no_alarm_icon);
}

GBitmap* get_alarm_icon_trans()
{
    return get_icon(RESOURCE_ID_ALARM_ICON_TRANS, alarm_icon_trans);
}

GBitmap* get_no_alarm_icon_trans()
{
    return get_icon(RESOURCE_ID_NO_ALARM_ICON_TRANS, no_alarm_icon_trans);
}

GBitmap* get_edit_icon()
{
    return get_icon(RESOURCE_ID_EDIT_ICON, edit_icon);
}

GBitmap* get_up_icon()
{
    return get_icon(RESOURCE_ID_UP_ICON, up_icon);
}

GBitmap* get_down_icon()
{
    return get_icon(RESOURCE_ID_DOWN_ICON, down_icon);
}

GBitmap* get_play_icon()
{
    return get_icon(RESOURCE_ID_PLAY_ICON, play_icon);
}

GBitmap* get_snooze_icon()
{
    return get_icon(RESOURCE_ID_SNOOZE_ICON, snooze_icon);
}

GBitmap* get_silence_icon()
{
    return get_icon(RESOURCE_ID_SILENCE_ICON, silence_icon);
}

void destroy_all_icons()
{
    destroy_icon(check_icon);
    destroy_icon(config_icon);
    destroy_icon(alarm_icon);
    destroy_icon(no_alarm_icon);
    destroy_icon(alarm_icon_trans);
    destroy_icon(no_alarm_icon_trans);
    destroy_icon(edit_icon);
    destroy_icon(up_icon);
    destroy_icon(down_icon);
    destroy_icon(play_icon);
    destroy_icon(snooze_icon);
    destroy_icon(silence_icon);
}
