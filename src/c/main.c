#include <pebble.h>
#include "face.h"
#include "parallax.h"
#include "settings.h"

static Window *s_window;

static void on_tick(struct tm *tick_time, TimeUnits units_changed) {
  (void)tick_time;
  (void)units_changed;
  face_tick();
}

static void resubscribe_tick(void) {
  tick_timer_service_unsubscribe();
  TimeUnits units = settings_get()->show_seconds ? SECOND_UNIT : MINUTE_UNIT;
  tick_timer_service_subscribe(units, on_tick);
}

static void on_settings_changed(void) {
  parallax_apply_settings();
  resubscribe_tick();
  face_apply_settings();
}

static void on_parallax_changed(void) {
  face_on_parallax_changed();
}

static void window_load(Window *window) {
  face_init(window);
}

static void window_unload(Window *window) {
  (void)window;
  face_deinit();
}

static void init(void) {
  settings_init();
  settings_set_changed_callback(on_settings_changed);

  parallax_set_changed_callback(on_parallax_changed);
  parallax_init();

  s_window = window_create();
  window_set_background_color(s_window, GColorBlack);
  window_set_window_handlers(s_window, (WindowHandlers){
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(s_window, true);

  resubscribe_tick();
}

static void deinit(void) {
  tick_timer_service_unsubscribe();
  parallax_deinit();
  settings_deinit();
  window_destroy(s_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
