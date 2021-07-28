#pragma once

void init_battery_indicator(GRect batteryLayerBounds);
void init_update_layers(TextLayer *time_layer, TextLayer *week_numer_layer, TextLayer *week_day_layer, TextLayer *date_layer, Layer *battery_layer, TextLayer *steps_layer);

void update_all();
void update_battery_layer(Layer *layer, GContext* ctx);

void on_time_tick(struct tm *tick_time, TimeUnits units_changed);
void on_battery_state_changed(BatteryChargeState charge_state);
void on_health_event(HealthEventType event, void *context);