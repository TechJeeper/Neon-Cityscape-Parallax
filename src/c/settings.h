#pragma once

#include <pebble.h>
#include <stdbool.h>

typedef enum {
  TIME_FMT_SYSTEM = 0,
  TIME_FMT_12H,
  TIME_FMT_24H
} TimeFormat;

typedef enum {
  PARALLAX_LOW = 0,
  PARALLAX_MED,
  PARALLAX_HIGH
} ParallaxIntensity;

typedef enum {
  THEME_MAGENTA = 0,
  THEME_CYAN,
  THEME_AMBER
} NeonTheme;

typedef struct {
  TimeFormat time_format;
  bool leading_zero;
  bool show_date;
  bool show_seconds;
  bool parallax_enabled;
  ParallaxIntensity parallax_intensity;
  NeonTheme neon_theme;
  bool invert_bw;
} FaceSettings;

typedef void (*SettingsChangedFn)(void);

void settings_init(void);
void settings_deinit(void);
void settings_set_changed_callback(SettingsChangedFn cb);
const FaceSettings *settings_get(void);
