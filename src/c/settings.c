#include "settings.h"

enum {
  PERSIST_TIME_FORMAT = 1,
  PERSIST_LEADING_ZERO = 2,
  PERSIST_SHOW_DATE = 3,
  PERSIST_SHOW_SECONDS = 4,
  PERSIST_PARALLAX = 5,
  PERSIST_INTENSITY = 6,
  PERSIST_THEME = 7,
  PERSIST_INVERT_BW = 8,
};

static FaceSettings s_settings;
static SettingsChangedFn s_changed;
static bool s_inbox_open;

static int clamp_int(int v, int lo, int hi) {
  if (v < lo) return lo;
  if (v > hi) return hi;
  return v;
}

static void normalize(void) {
  s_settings.time_format = (TimeFormat)clamp_int(
      (int)s_settings.time_format, TIME_FMT_SYSTEM, TIME_FMT_24H);
  s_settings.parallax_intensity = (ParallaxIntensity)clamp_int(
      (int)s_settings.parallax_intensity, PARALLAX_LOW, PARALLAX_HIGH);
  s_settings.neon_theme = (NeonTheme)clamp_int(
      (int)s_settings.neon_theme, THEME_MAGENTA, THEME_AMBER);
}

static void settings_set_defaults(void) {
  s_settings.time_format = TIME_FMT_SYSTEM;
  s_settings.leading_zero = true;
  s_settings.show_date = true;
  s_settings.show_seconds = false;
  s_settings.parallax_enabled = true;
  s_settings.parallax_intensity = PARALLAX_MED;
  s_settings.neon_theme = THEME_CYAN;
  s_settings.invert_bw = false;
}

static void settings_load(void) {
  settings_set_defaults();
  if (persist_exists(PERSIST_TIME_FORMAT)) {
    s_settings.time_format = (TimeFormat)persist_read_int(PERSIST_TIME_FORMAT);
  }
  if (persist_exists(PERSIST_LEADING_ZERO)) {
    s_settings.leading_zero = persist_read_bool(PERSIST_LEADING_ZERO);
  }
  if (persist_exists(PERSIST_SHOW_DATE)) {
    s_settings.show_date = persist_read_bool(PERSIST_SHOW_DATE);
  }
  if (persist_exists(PERSIST_SHOW_SECONDS)) {
    s_settings.show_seconds = persist_read_bool(PERSIST_SHOW_SECONDS);
  }
  if (persist_exists(PERSIST_PARALLAX)) {
    s_settings.parallax_enabled = persist_read_bool(PERSIST_PARALLAX);
  }
  if (persist_exists(PERSIST_INTENSITY)) {
    s_settings.parallax_intensity =
        (ParallaxIntensity)persist_read_int(PERSIST_INTENSITY);
  }
  if (persist_exists(PERSIST_THEME)) {
    s_settings.neon_theme = (NeonTheme)persist_read_int(PERSIST_THEME);
  }
  if (persist_exists(PERSIST_INVERT_BW)) {
    s_settings.invert_bw = persist_read_bool(PERSIST_INVERT_BW);
  }
  normalize();
}

static void settings_save(void) {
  normalize();
  persist_write_int(PERSIST_TIME_FORMAT, (int)s_settings.time_format);
  persist_write_bool(PERSIST_LEADING_ZERO, s_settings.leading_zero);
  persist_write_bool(PERSIST_SHOW_DATE, s_settings.show_date);
  persist_write_bool(PERSIST_SHOW_SECONDS, s_settings.show_seconds);
  persist_write_bool(PERSIST_PARALLAX, s_settings.parallax_enabled);
  persist_write_int(PERSIST_INTENSITY, (int)s_settings.parallax_intensity);
  persist_write_int(PERSIST_THEME, (int)s_settings.neon_theme);
  persist_write_bool(PERSIST_INVERT_BW, s_settings.invert_bw);
}

static int tuple_as_int(Tuple *t, int fallback) {
  if (!t) return fallback;
  if (t->type == TUPLE_INT) return (int)t->value->int32;
  if (t->type == TUPLE_CSTRING) return atoi(t->value->cstring);
  return fallback;
}

static bool tuple_as_bool(Tuple *t, bool fallback) {
  if (!t) return fallback;
  if (t->type == TUPLE_INT) return t->value->int32 != 0;
  if (t->type == TUPLE_CSTRING) {
    return strcmp(t->value->cstring, "true") == 0 ||
           strcmp(t->value->cstring, "1") == 0;
  }
  return fallback;
}

static void inbox_received(DictionaryIterator *iter, void *context) {
  (void)context;
  Tuple *t;

  t = dict_find(iter, MESSAGE_KEY_TimeFormat);
  if (t) {
    s_settings.time_format = (TimeFormat)tuple_as_int(t, (int)s_settings.time_format);
  }
  t = dict_find(iter, MESSAGE_KEY_LeadingZero);
  if (t) {
    s_settings.leading_zero = tuple_as_bool(t, s_settings.leading_zero);
  }
  t = dict_find(iter, MESSAGE_KEY_ShowDate);
  if (t) {
    s_settings.show_date = tuple_as_bool(t, s_settings.show_date);
  }
  t = dict_find(iter, MESSAGE_KEY_ShowSeconds);
  if (t) {
    s_settings.show_seconds = tuple_as_bool(t, s_settings.show_seconds);
  }
  t = dict_find(iter, MESSAGE_KEY_ParallaxEnabled);
  if (t) {
    s_settings.parallax_enabled = tuple_as_bool(t, s_settings.parallax_enabled);
  }
  t = dict_find(iter, MESSAGE_KEY_ParallaxIntensity);
  if (t) {
    s_settings.parallax_intensity =
        (ParallaxIntensity)tuple_as_int(t, (int)s_settings.parallax_intensity);
  }
  t = dict_find(iter, MESSAGE_KEY_NeonTheme);
  if (t) {
    s_settings.neon_theme = (NeonTheme)tuple_as_int(t, (int)s_settings.neon_theme);
  }
  t = dict_find(iter, MESSAGE_KEY_InvertBw);
  if (t) {
    s_settings.invert_bw = tuple_as_bool(t, s_settings.invert_bw);
  }

  settings_save();
  if (s_changed) {
    s_changed();
  }
}

void settings_init(void) {
  settings_load();
  if (!s_inbox_open) {
    app_message_register_inbox_received(inbox_received);
    app_message_open(app_message_inbox_size_maximum(),
                     app_message_outbox_size_maximum());
    s_inbox_open = true;
  }
}

void settings_deinit(void) {
  settings_save();
  if (s_inbox_open) {
    app_message_deregister_callbacks();
    s_inbox_open = false;
  }
}

void settings_set_changed_callback(SettingsChangedFn cb) {
  s_changed = cb;
}

const FaceSettings *settings_get(void) {
  return &s_settings;
}
