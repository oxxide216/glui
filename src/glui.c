#include "glui.h"
#include "winx/src/event.h"

Glui glui_init(WinxWindow *window) {
  Glui glui = {0};

  Vec2 size = vec2((f32) window->width, (f32) window->height);
  glui.renderer = glui_init_renderer(size);
  glui.layout = glui_init_layout(size);
  glui.window = window;

  return glui;
}

void glui_next_frame(Glui *glui) {
  glui_render(&glui->renderer);
  glui_reset_layout(&glui->layout);

  if (glui->renderer.size.x != (f32) glui->window->width) {
    glui->renderer.size.x = (f32) glui->window->width;
    glui->layout.size.x = (f32) glui->window->width;
    glui->renderer.redraw = true;
  }

  if (glui->renderer.size.y != (f32) glui->window->height) {
    glui->renderer.size.y = (f32) glui->window->height;
    glui->layout.size.y = (f32) glui->window->height;
    glui->renderer.redraw = true;
  }

  glui->events.len = 0;
  WinxEvent event;
  while ((event = winx_get_event(glui->window, false)).kind != WinxEventKindNone)
    DA_APPEND(glui->events, event);
}

GluiStyle *glui_get_style(Glui *glui, Str class) {
  for (u32 i = 0; i < glui->styles.len; ++i)
    if (str_eq(glui->styles.items[i].class, class))
      return glui->styles.items + i;

  GluiStyle style = {0};
  style.class = class;
  DA_APPEND(glui->styles, style);

  return glui->styles.items + glui->styles.len - 1;
}

static WinxEvent glui_get_event_of_kind(Glui *glui, WinxEventKind kind) {
  for (u32 i = 0; i < glui->events.len; ++i) {
    if (glui->events.items[i].kind == kind) {
      WinxEvent event = glui->events.items[i];
      return event;
    }
  }

  return (WinxEvent) { WinxEventKindNone, {} };
}

bool glui_button(Glui *glui, Str text, Vec4 color, Vec2 size) {
  Vec4 bounds = glui_compute_bounds(&glui->layout, size, false);
  glui_push_quad(&glui->renderer, bounds, color);

  (void) text;

  WinxEvent event = glui_get_event_of_kind(glui, WinxEventKindButtonPress);

  if (event.kind != WinxEventKindButtonPress)
    return false;

  f32 x = (f32) event.as.button_press.x;
  f32 y = (f32) event.as.button_press.y;

  if (x < bounds.x || x > bounds.x + bounds.z ||
      y < bounds.y || y > bounds.y + bounds.w)
    return false;

  return true;
}

void glui_begin_block(Glui *glui, Vec2 margin, GluiAnchor anchor,
                      Vec4 color, Vec2 size) {
  Vec4 bounds = glui_compute_bounds(&glui->layout, size, false);
  glui_push_quad(&glui->renderer, bounds, color);
  glui_push_block(&glui->layout, bounds, margin, anchor);
}

void glui_end_block(Glui *glui) {
  glui_pop_block(&glui->layout);
}
