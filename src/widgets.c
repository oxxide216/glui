#include <stddef.h>

#include "widgets.h"
#include "shl_arena.h"

u32 sdbm_hash(u8 *data, u32 size) {
  u32 hash = 0;

  for (u32 i = 0; i < size; ++i)
    hash = (data[i]) + (hash << 6) + (hash << 16) - hash;

  return hash;
}

static bool glui_widget_eq(GluiWidget *a, GluiWidget *b) {
  if (a->kind != b->kind)
    return false;

  switch (a->kind) {
  case GluiWidgetKindButton: {
    if (!str_eq(a->as.button.text, b->as.button.text))
      return false;
  } break;
  }

  if (a->config.event != b->config.event)
    return false;

  if (a->config.anchor != b->config.anchor)
    return false;

  for (u32 i = 0; i < 4; ++i)
    if (a->config.margin_values[i] != b->config.margin_values[i])
      return false;

  return true;
}

static bool glui_contains_widget(Glui *glui, GluiWidget *widget) {
  u32 index = sdbm_hash((u8 *) widget, offsetof(GluiWidget, as));

  GluiPair *pair = glui->pairs[index % GLUI_PAIRS_CAP];
  while (pair) {
    if (glui_widget_eq(glui->widgets.items + pair->index, widget))
      return true;

    pair = pair->next;
  }

  return false;
}

static void glui_add_widget(Glui *glui, GluiWidget *widget) {
  u32 index = sdbm_hash((u8 *) widget, offsetof(GluiWidget, as));

  GluiPair **next = glui->pairs + index % GLUI_PAIRS_CAP;
  GluiPair *pair = *next;
  while (pair) {
    next = &pair->next;
    pair = pair->next;
  }

  *next = aalloc(sizeof(GluiPair));
  (*next)->index = glui->widgets.len;
  (*next)->next = NULL;

  DA_APPEND(glui->widgets, *widget);
}

void glui_button(Glui *glui, Str text, GluiWidgetConfig *config) {
  GluiWidget widget = { *config, GluiWidgetKindButton, {}, {}, {}, {} };

  if (glui_contains_widget(glui, &widget))
    return;

  GluiWidgetButton button = {0};
  button.text = text;
  button.body = glass_init_object(&glui->general_shader);

  widget.as.button = button;

  widget.pos = vec2(50.0, 50.0);
  widget.size = vec2(200.0, 200.0);
  widget.color = vec4(1.0, 0.0, 1.0, 1.0);

  glui_add_widget(glui, &widget);
}
