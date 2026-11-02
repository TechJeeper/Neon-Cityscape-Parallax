#include "face.h"
#include "parallax.h"
#include "settings.h"

static Window *s_window;
static Layer *s_layer;

static GBitmap *s_bg;
static GBitmap *s_mid;
static GBitmap *s_fg;

static int s_hour;
static int s_minute;
static int s_second;
static int s_wday;
static int s_mday;
static int s_mon;

static GColor theme_fg(void) {
#if defined(PBL_COLOR)
  switch (settings_get()->neon_theme) {
    case THEME_MAGENTA: return GColorMagenta;
    case THEME_AMBER:   return GColorChromeYellow;
    case THEME_CYAN:
    default:            return GColorCyan;
  }
#else
  return settings_get()->invert_bw ? GColorBlack : GColorWhite;
#endif
}

static GColor theme_bg(void) {
#if defined(PBL_COLOR)
  return GColorBlack;
#else
  return settings_get()->invert_bw ? GColorWhite : GColorBlack;
#endif
}

static bool use_24h(void) {
  const FaceSettings *s = settings_get();
  if (s->time_format == TIME_FMT_24H) return true;
  if (s->time_format == TIME_FMT_12H) return false;
  return clock_is_24h_style();
}

static void format_time(char *buf, size_t n) {
  int hour = s_hour;
  const bool is24 = use_24h();
  if (!is24) {
    hour = hour % 12;
    if (hour == 0) hour = 12;
  }
  const FaceSettings *cfg = settings_get();
  if (cfg->show_seconds) {
    if (cfg->leading_zero) {
      snprintf(buf, n, "%02d:%02d:%02d", hour, s_minute, s_second);
    } else {
      snprintf(buf, n, "%d:%02d:%02d", hour, s_minute, s_second);
    }
  } else if (cfg->leading_zero) {
    snprintf(buf, n, "%02d:%02d", hour, s_minute);
  } else {
    snprintf(buf, n, "%d:%02d", hour, s_minute);
  }
}

static void format_date(char *buf, size_t n) {
  static const char *const DOW[] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
  };
  static const char *const MON[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
  };
  snprintf(buf, n, "%s %d %s", DOW[s_wday % 7], s_mday, MON[s_mon % 12]);
}

static void draw_layer_bitmap(GContext *ctx, GBitmap *bmp, GRect bounds,
                              int16_t ox, int16_t oy, bool bottom_align) {
  if (!bmp) return;
  GSize sz = gbitmap_get_bounds(bmp).size;
  int x = (bounds.size.w - sz.w) / 2 + ox;
  int y;
  if (bottom_align) {
    y = bounds.size.h - sz.h + oy;
  } else {
    y = (bounds.size.h - sz.h) / 2 + oy;
  }

#if defined(PBL_COLOR)
  graphics_context_set_compositing_mode(ctx, GCompOpSet);
#else
  graphics_context_set_compositing_mode(ctx,
      settings_get()->invert_bw ? GCompOpAssignInverted : GCompOpAssign);
#endif
  graphics_draw_bitmap_in_rect(ctx, bmp, GRect(x, y, sz.w, sz.h));
}

static void update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  const FaceSettings *cfg = settings_get();
  const ParallaxOffsets *p = parallax_get();
  GColor fg = theme_fg();
  GColor bg = theme_bg();

  graphics_context_set_fill_color(ctx, bg);
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);

  // Far sky shifts least; rooftops shift most — faux-3D parallax.
  draw_layer_bitmap(ctx, s_bg, bounds, p->bg_x, p->bg_y, false);
  draw_layer_bitmap(ctx, s_mid, bounds, p->mid_x, p->mid_y, true);
  draw_layer_bitmap(ctx, s_fg, bounds, p->fg_x, p->fg_y + 2, true);

  char time_buf[16];
  format_time(time_buf, sizeof(time_buf));
  graphics_context_set_text_color(ctx, fg);

  const GFont time_font = cfg->show_seconds
      ? fonts_get_system_font(FONT_KEY_LECO_28_LIGHT_NUMBERS)
      : fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS);
  const int time_h = cfg->show_seconds ? 32 : 44;
  graphics_draw_text(ctx, time_buf, time_font,
                     GRect(0, 4, bounds.size.w, time_h),
                     GTextOverflowModeTrailingEllipsis,
                     GTextAlignmentCenter, NULL);

  if (cfg->show_date) {
    char date_buf[24];
    format_date(date_buf, sizeof(date_buf));
    graphics_draw_text(ctx, date_buf,
                       fonts_get_system_font(FONT_KEY_GOTHIC_14),
                       GRect(0, 4 + time_h - 2, bounds.size.w, 18),
                       GTextOverflowModeTrailingEllipsis,
                       GTextAlignmentCenter, NULL);
  }
}

static void mark_dirty(void) {
  if (s_layer) {
    layer_mark_dirty(s_layer);
  }
  if (s_window) {
    window_set_background_color(s_window, theme_bg());
  }
}

void face_init(Window *window) {
  s_window = window;
  s_bg = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG_SKY);
  s_mid = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MID_SKYLINE);
  s_fg = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_FG_ROOFTOPS);

  Layer *root = window_get_root_layer(window);
  s_layer = layer_create(layer_get_bounds(root));
  layer_set_update_proc(s_layer, update_proc);
  layer_add_child(root, s_layer);

  face_tick();
  mark_dirty();
}

void face_deinit(void) {
  if (s_layer) {
    layer_destroy(s_layer);
    s_layer = NULL;
  }
  if (s_bg) { gbitmap_destroy(s_bg); s_bg = NULL; }
  if (s_mid) { gbitmap_destroy(s_mid); s_mid = NULL; }
  if (s_fg) { gbitmap_destroy(s_fg); s_fg = NULL; }
  s_window = NULL;
}

void face_tick(void) {
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  s_hour = t->tm_hour;
  s_minute = t->tm_min;
  s_second = t->tm_sec;
  s_wday = t->tm_wday;
  s_mday = t->tm_mday;
  s_mon = t->tm_mon;
  mark_dirty();
}

void face_apply_settings(void) {
  mark_dirty();
}

void face_on_parallax_changed(void) {
  mark_dirty();
}
