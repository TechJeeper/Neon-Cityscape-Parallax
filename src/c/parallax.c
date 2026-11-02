#include "parallax.h"

#define ACCEL_SAMPLES 2

static ParallaxOffsets s_off;
static ParallaxChangedFn s_changed;
static bool s_subscribed;
static int16_t s_ax;
static int16_t s_ay;

static int16_t clamp16(int32_t v, int16_t lo, int16_t hi) {
  if (v < lo) return lo;
  if (v > hi) return hi;
  return (int16_t)v;
}

static int intensity_scale(void) {
  switch (settings_get()->parallax_intensity) {
    case PARALLAX_LOW:  return 1;
    case PARALLAX_HIGH: return 3;
    case PARALLAX_MED:
    default:            return 2;
  }
}

static void recompute(void) {
  if (!settings_get()->parallax_enabled) {
    s_off.bg_x = s_off.bg_y = 0;
    s_off.mid_x = s_off.mid_y = 0;
    s_off.fg_x = s_off.fg_y = 0;
    return;
  }

  // Wrist tilt: X = roll (left/right), Y = pitch (toward/away).
  // Map ~±1000 mg into small pixel shifts; nearer layers move more.
  const int scale = intensity_scale();
  const int32_t ax = s_ax;
  const int32_t ay = s_ay;

  s_off.bg_x = clamp16((ax * 1 * scale) / 500, -4, 4);
  s_off.bg_y = clamp16((ay * 1 * scale) / 700, -2, 2);

  s_off.mid_x = clamp16((ax * 2 * scale) / 400, -8, 8);
  s_off.mid_y = clamp16((ay * 2 * scale) / 550, -4, 4);

  s_off.fg_x = clamp16((ax * 3 * scale) / 300, -12, 12);
  s_off.fg_y = clamp16((ay * 3 * scale) / 450, -6, 6);
}

static void maybe_notify(const ParallaxOffsets *prev) {
  if (prev->bg_x == s_off.bg_x && prev->bg_y == s_off.bg_y &&
      prev->mid_x == s_off.mid_x && prev->mid_y == s_off.mid_y &&
      prev->fg_x == s_off.fg_x && prev->fg_y == s_off.fg_y) {
    return;
  }
  if (s_changed) {
    s_changed();
  }
}

static void accel_handler(AccelData *data, uint32_t num_samples) {
  int32_t sx = 0, sy = 0;
  uint32_t used = 0;
  for (uint32_t i = 0; i < num_samples; i++) {
    if (data[i].did_vibrate) continue;
    sx += data[i].x;
    sy += data[i].y;
    used++;
  }
  if (used == 0) return;

  s_ax = (int16_t)(sx / (int32_t)used);
  s_ay = (int16_t)(sy / (int32_t)used);

  ParallaxOffsets prev = s_off;
  recompute();
  maybe_notify(&prev);
}

static void subscribe(void) {
  if (s_subscribed) return;
  if (!settings_get()->parallax_enabled) return;
  accel_data_service_subscribe(ACCEL_SAMPLES, accel_handler);
  accel_service_set_sampling_rate(ACCEL_SAMPLING_10HZ);
  s_subscribed = true;
}

static void unsubscribe(void) {
  if (!s_subscribed) return;
  accel_data_service_unsubscribe();
  s_subscribed = false;
}

void parallax_init(void) {
  s_off.bg_x = s_off.bg_y = 0;
  s_off.mid_x = s_off.mid_y = 0;
  s_off.fg_x = s_off.fg_y = 0;
  subscribe();
}

void parallax_deinit(void) {
  unsubscribe();
}

void parallax_apply_settings(void) {
  ParallaxOffsets prev = s_off;
  if (settings_get()->parallax_enabled) {
    subscribe();
  } else {
    unsubscribe();
  }
  recompute();
  maybe_notify(&prev);
}

const ParallaxOffsets *parallax_get(void) {
  return &s_off;
}

void parallax_set_changed_callback(ParallaxChangedFn cb) {
  s_changed = cb;
}
