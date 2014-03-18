#include <pebble.h>

static Window *window;
static TextLayer *time_layer;
static Layer *departure_layer;
static TextLayer *departure_time_layer;
static TextLayer *departure_delay_layer;
static TextLayer *departure_platform_layer;

static struct DepartureInfo {
  char time[10];
  char delay[10];
  char platform[10];
} info = { .time = "12:55", .delay = "+5", .platform = "1A"};

static void handle_departure(struct DepartureInfo info){
  static char departure_time_text[10]; strcpy(departure_time_text, info.time);
  static char departure_delay_text[10]; strcpy(departure_delay_text, info.delay);
  static char departure_platform_text[10]; strcpy(departure_platform_text, info.platform);

  text_layer_set_text(departure_time_layer, departure_time_text);
  text_layer_set_text(departure_delay_layer, departure_delay_text);
  text_layer_set_text(departure_platform_layer, departure_platform_text);
}

static void handle_second_tick(struct tm* tick_time, TimeUnits units_changed) {
  static char time_text[] = "00:00:00";

  strftime(time_text, sizeof(time_text), "%T", tick_time);
  text_layer_set_text(time_layer, time_text);
}

static void configure_layer(TextLayer *layer, char *key) {
  text_layer_set_text_color(layer, GColorWhite);
  text_layer_set_background_color(layer, GColorClear);
  text_layer_set_font(layer, fonts_get_system_font(key));
  text_layer_set_text_alignment(layer, GTextAlignmentCenter);
}

static void set_initial_time() {
  time_t now = time(NULL);
  struct tm *current_time = localtime(&now);
  handle_second_tick(current_time, SECOND_UNIT);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  window_set_background_color(window, GColorBlack);

  departure_layer = layer_create((GRect) { .origin = { 0, 0 }, .size = { bounds.size.w, 54 } });
  layer_add_child(window_layer, departure_layer);

  departure_time_layer = text_layer_create((GRect) { .origin = { 0, 0 }, .size = { bounds.size.w/3, 54 } });
  configure_layer(departure_time_layer, FONT_KEY_GOTHIC_24_BOLD);
  layer_add_child(departure_layer, text_layer_get_layer(departure_time_layer));

  departure_delay_layer = text_layer_create((GRect) { .origin = { bounds.size.w/3, 0 }, .size = { bounds.size.w/3, 54 } });
  configure_layer(departure_delay_layer, FONT_KEY_GOTHIC_24_BOLD);
  layer_add_child(departure_layer, text_layer_get_layer(departure_delay_layer));

  departure_platform_layer = text_layer_create((GRect) { .origin = { 2*bounds.size.w/3, 0 }, .size = { bounds.size.w/3, 54 } });
  configure_layer(departure_platform_layer, FONT_KEY_GOTHIC_24_BOLD);
  layer_add_child(departure_layer, text_layer_get_layer(departure_platform_layer));

  time_layer = text_layer_create((GRect) { .origin = { 0, 72 }, .size = { bounds.size.w, 54 } });
  configure_layer(time_layer, FONT_KEY_BITHAM_34_MEDIUM_NUMBERS);
  layer_add_child(window_layer, text_layer_get_layer(time_layer));

  handle_departure(info);
  set_initial_time();
  tick_timer_service_subscribe(SECOND_UNIT, &handle_second_tick);
}

static void window_unload(Window *window) {
  text_layer_destroy(time_layer);
}

static void init(void) {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
