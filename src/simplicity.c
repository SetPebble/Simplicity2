/* Matthew Clark, SetPebble.com
 * based on Simplicity by Pebble
 */

#include <pebble.h>

//  constants

enum Settings { setting_screen = 1, setting_date, setting_vibrate };
const char* kDateFormat[] = { "%b %e", "%e %b" };

//  variables

static Window* window;
static TextLayer* text_date_layer;
static TextLayer* text_time_layer;
static BitmapLayer* line_layer;
static InverterLayer* inverter_layer;
static char time_text[] = "00:00", date_text[] = "Xxxxxxxxxxxxxxxxxxxxxxx 00";
static enum SettingScreen { screen_black = 0, screen_white, screen_count } screen;
static enum SettingDate { date_month_day = 0, date_day_month, date_count } date;
static enum SettingVibrate { vibrate_none = 0, vibrate_hourly, vibrate_count } vibrate;
static AppSync app;
static uint8_t buffer[256];

//  methods

void display_time(struct tm* pbltime) {

  //  check for empty time

  time_t now;
  if (pbltime == NULL) {
    now = time(NULL);
    pbltime = localtime(&now);
  }

  //  display date

  strftime(date_text, sizeof(date_text), kDateFormat[date], pbltime);
  text_layer_set_text(text_date_layer, date_text);

  //  display time

  strftime(time_text, sizeof(time_text), clock_is_24h_style() ? "%R" : "%I:%M", pbltime);
  //  Kludge to handle lack of non-padded hour format string for twelve hour clock.
  if (!clock_is_24h_style() && (time_text[0] == '0'))
    memmove(time_text, &time_text[1], sizeof(time_text) - 1);
  text_layer_set_text(text_time_layer, time_text);
}

static void tuple_changed_callback(const uint32_t key, const Tuple* tuple_new, const Tuple* tuple_old, void* context) {
  //  we know these values are uint8 format
  int value = tuple_new->value->uint8;
  switch (key) {
    case setting_screen:
      if ((value >= 0) && (value < screen_count) && (screen != value)) {
        //  update value
        screen = value;
        //  relocate inverter layer
        GRect rect = layer_get_frame(inverter_layer_get_layer(inverter_layer));
        rect.origin.x = (screen == screen_black) ? 144 : 0;
        layer_set_frame(inverter_layer_get_layer(inverter_layer), rect);
      }
      break;
    case setting_date:
      if ((value >= 0) && (value < date_count) && (date != value)) {
        //  update value
        date = value;
        //  redraw date
        display_time(NULL);
      }
      break;
    case setting_vibrate:
      if ((value >= 0) && (value < vibrate_count) && (vibrate != value))
        //  update value
        vibrate = value;
      break;
  }
}

static void app_error_callback(DictionaryResult dict_error, AppMessageResult app_message_error, void* context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "app error %d", app_message_error);
}

static void handle_tick(struct tm* tick_time, TimeUnits units_changed) {
  //  update time
  display_time(tick_time);
  //  check for hourly vibration notification
  if ((vibrate == vibrate_hourly) && (units_changed & HOUR_UNIT))
    vibes_short_pulse();
}

void handle_init(void) {
  //  initialize window
  window = window_create();
  window_set_background_color(window, GColorBlack);
  window_stack_push(window, true);
  //  default settings
  screen = screen_black;
  date = date_month_day;
  vibrate = vibrate_none;
  //  root layer
  Layer* root_layer = window_get_root_layer(window);
  //  date
  text_date_layer = text_layer_create(GRect(8, 68, 144 - 8, 168 - 68));
  text_layer_set_text_color(text_date_layer, GColorWhite);
  text_layer_set_background_color(text_date_layer, GColorClear);
  text_layer_set_font(text_date_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_CONDENSED_21)));
  layer_add_child(root_layer, text_layer_get_layer(text_date_layer));
  //  time
  text_time_layer = text_layer_create(GRect(7, 92, 144 - 7, 168 - 92));
  text_layer_set_text_color(text_time_layer, GColorWhite);
  text_layer_set_background_color(text_time_layer, GColorClear);
  text_layer_set_font(text_time_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_BOLD_SUBSET_49)));
  layer_add_child(root_layer, text_layer_get_layer(text_time_layer));
  //  line
  line_layer = bitmap_layer_create(GRect(8, 97, 123, 1));
  bitmap_layer_set_background_color(line_layer, GColorWhite);
  layer_add_child(root_layer, bitmap_layer_get_layer(line_layer));
  //  inverter
  inverter_layer = inverter_layer_create(GRect((screen == screen_black) ? 144 : 0, 0, 144, 168));
  layer_add_child(window_get_root_layer(window), inverter_layer_get_layer(inverter_layer));
  //  app communication
  Tuplet tuples[] = {
    TupletInteger(setting_screen, screen),
    TupletInteger(setting_date, date),
    TupletInteger(setting_vibrate, vibrate)
  };
  app_message_open(160, 160);
  app_sync_init(&app, buffer, sizeof(buffer), tuples, ARRAY_LENGTH(tuples),
                tuple_changed_callback, app_error_callback, NULL);
  //  display time (immediately before first tick event)
  display_time(NULL);
  //  tick service
  tick_timer_service_subscribe(MINUTE_UNIT, handle_tick);
}

void handle_deinit(void) {
  //  tick unsubscribe
  tick_timer_service_unsubscribe();
  //  inverter
  inverter_layer_destroy(inverter_layer);
  //  app unsync
  app_sync_deinit(&app);
  //  release display objects
  text_layer_destroy(text_date_layer);
  text_layer_destroy(text_time_layer);
  bitmap_layer_destroy(line_layer);
  //  destroy window
  window_destroy(window);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
