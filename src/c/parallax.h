#pragma once

#include <pebble.h>
#include "settings.h"

typedef struct {
  int16_t bg_x;
  int16_t bg_y;
  int16_t mid_x;
  int16_t mid_y;
  int16_t fg_x;
  int16_t fg_y;
} ParallaxOffsets;

void parallax_init(void);
void parallax_deinit(void);
void parallax_apply_settings(void);
const ParallaxOffsets *parallax_get(void);

typedef void (*ParallaxChangedFn)(void);
void parallax_set_changed_callback(ParallaxChangedFn cb);
