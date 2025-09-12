#include "glui.h"
#include "winx/src/event.h"
#include "widgets.h"
#define SHL_ARENA_IMPLEMENTATION
#include "shl_arena.h"

#define TEXT_EDITOR_TAB_WIDTH 2

#define EVENT_MASK(event_kind) (1 << (event_kind))

static void glui_init_root_list(Glui *glui) {
  glui->root_widget = aalloc(sizeof(GluiWidget));
  *glui->root_widget = (GluiWidget) {0};
  glui->current_list = glui->root_widget;

  glui->current_list->kind = GluiWidgetKindList;
  glui->current_list->id = (GluiWidgetId) { __FILE__, __LINE__ };
  glui->current_list->style = *glui_get_style(glui, "root");
  glui->current_list->bounds = vec4(0.0, 0.0, glui->size.x, glui->size.y);
  glui->current_list->parent = NULL;

  glui->current_list->as.list.kind = GluiListKindVertical;
  glui->current_list->as.list.margin = vec2(0.0, 0.0);
}

Glui glui_init(WinxWindow *window, char *font_file_path) {
  Glui glui = {0};

  Vec2 size = vec2((f32) window->width, (f32) window->height);

  glui.size = size;
  glui.renderer = glui_init_renderer(size, font_file_path);
  glui.window = window;

  glui_init_root_list(&glui);

  return glui;
}

static void glui_compute_bounds(GluiWidget *root_widget, Vec4 bounds) {
  if (!root_widget->are_bounds_abs)
    root_widget->bounds = bounds;

  if (root_widget->kind != GluiWidgetKindList)
    return;

  Vec4 free_space = vec4(root_widget->bounds.x + root_widget->as.list.margin.x,
                         root_widget->bounds.y + root_widget->as.list.margin.y,
                         root_widget->bounds.z - root_widget->as.list.margin.x * 2.0,
                         root_widget->bounds.w - root_widget->as.list.margin.y * 2.0);

  Vec2 offset = {0};
  if (root_widget->as.list.kind == GluiListKindHorizontal) {
    free_space.z = (free_space.z - root_widget->as.list.margin.x *
                    (root_widget->as.list.children.len - 1.0)) / root_widget->as.list.children.len;
    offset.x = free_space.z + root_widget->as.list.margin.x;
  } else {
    free_space.w = (free_space.w - root_widget->as.list.margin.y *
                    (root_widget->as.list.children.len - 1.0)) / root_widget->as.list.children.len;
    offset.y = free_space.w + root_widget->as.list.margin.y;
  }

  for (u32 i = 0; i < root_widget->as.list.children.len; ++i) {
    glui_compute_bounds(root_widget->as.list.children.items[i], free_space);

    free_space.x += offset.x;
    free_space.y += offset.y;
  }
}

void glui_next_frame(Glui *glui) {
  glui->current_list = glui->root_widget;
  glui->current_list->style = *glui_get_style(glui, "root");

  Vec4 root_widget_bounds = vec4(0.0, 0.0, glui->size.x, glui->size.y);
  glui_compute_bounds(glui->root_widget, root_widget_bounds);
  glui_render(&glui->renderer, glui->root_widget);

  if (glui->renderer.size.x != (f32) glui->window->width) {
    glui->size.x = (f32) glui->window->width;
    glui->renderer.size.x = (f32) glui->window->width;
    glui->renderer.redraw = true;
  }

  if (glui->renderer.size.y != (f32) glui->window->height) {
    glui->size.y = (f32) glui->window->height;
    glui->renderer.size.y = (f32) glui->window->height;
    glui->renderer.redraw = true;
  }

  glui->events.len = 0;
  WinxEvent event;
  while ((event = winx_get_event(glui->window, false)).kind != WinxEventKindNone)
    DA_APPEND(glui->events, event);
}

GluiStyle *glui_get_style(Glui *glui, char *class) {
  for (u32 i = 0; i < glui->styles.len; ++i)
    if (strcmp(glui->styles.items[i].class, class) == 0)
      return glui->styles.items + i;

  GluiStyle style = {0};
  style.class = class;
  DA_APPEND(glui->styles, style);

  return glui->styles.items + glui->styles.len - 1;
}

void glui_abs_bounds(Glui *glui, Vec4 bounds) {
  glui->current_abs_bounds = bounds;
  glui->are_bounds_abs = true;
}

static WinxEvent glui_get_event_of_kind(Glui *glui, u32 kind_mask) {
  for (u32 i = 0; i < glui->events.len; ++i)
    if (EVENT_MASK(glui->events.items[i].kind) & kind_mask)
      return glui->events.items[i];

  return (WinxEvent) { WinxEventKindNone, {} };
}

static GluiWidget *glui_get_widget_rec(GluiWidget *root_widget, char *file_name, u32 line) {
  if (strcmp(root_widget->id.file_name, file_name) == 0 &&
      root_widget->id.line == line)
    return root_widget;

  if (root_widget->kind != GluiWidgetKindList)
    return NULL;

  for (u32 i = 0; i < root_widget->as.list.children.len; ++i) {
    GluiWidget *child = root_widget->as.list.children.items[i];
    GluiWidget *widget = glui_get_widget_rec(child, file_name, line);
    if (widget)
      return widget;
  }

  return NULL;
}

static GluiWidget *glui_get_widget(GluiWidget *root_widget,
                                   GluiWidget *current_list,
                                   char *file_name, u32 line) {
  GluiWidget *widget = glui_get_widget_rec(root_widget, file_name, line);
  if (widget)
    return widget;

  widget = aalloc(sizeof(GluiWidget));
  *widget = (GluiWidget) {0};
  widget->id.file_name = file_name;
  widget->id.line = line;

  DA_APPEND(current_list->as.list.children, widget);

  return widget;
}

bool glui_button_id(Glui *glui, char *file_name, u32 line,
                    GluiWStr text, char *class) {
  (void) text;

  GluiWidget *widget = glui_get_widget(glui->root_widget,
                                       glui->current_list,
                                       file_name, line);
  if (widget->kind != GluiWidgetKindButton)
    return false;

  widget->kind = GluiWidgetKindButton;
  widget->style = *glui_get_style(glui, class);
  widget->are_bounds_abs = glui->are_bounds_abs;
  if (widget->are_bounds_abs)
    widget->bounds = glui->current_abs_bounds;
  widget->parent = glui->current_list;

  widget->as.button.text = text;

  glui->are_bounds_abs = false;

  WinxEvent event = glui_get_event_of_kind(glui, EVENT_MASK(WinxEventKindButtonPress));
  if (event.kind == WinxEventKindButtonPress) {
    f32 x = (f32) event.as.button_press.x;
    f32 y = (f32) event.as.button_press.y;

    if (x >= widget->bounds.x && x <= widget->bounds.x + widget->bounds.z &&
        y >= widget->bounds.y && y <= widget->bounds.y + widget->bounds.w)
      widget->as.button.pressed = true;
  } else if (widget->as.button.pressed) {
    event = glui_get_event_of_kind(glui, EVENT_MASK(WinxEventKindButtonRelease));
    if (event.kind == WinxEventKindButtonRelease) {
      widget->as.button.pressed = false;

      f32 x = (f32) event.as.button_release.x;
      f32 y = (f32) event.as.button_release.y;

      if (x >= widget->bounds.x && x <= widget->bounds.x + widget->bounds.z &&
          y >= widget->bounds.y && y <= widget->bounds.y + widget->bounds.w)
        return true;
    }
  }

  return false;
}

void glui_begin_list_id(Glui *glui, char *file_name, u32 line,
                        GluiListKind kind, Vec2 margin, char *class) {
  GluiWidget *widget = glui_get_widget(glui->root_widget,
                                       glui->current_list,
                                       file_name, line);

  widget->kind = GluiWidgetKindList;
  widget->style = *glui_get_style(glui, class);
  widget->are_bounds_abs = glui->are_bounds_abs;
  if (widget->are_bounds_abs)
    widget->bounds = glui->current_abs_bounds;
  widget->parent = glui->current_list;

  widget->as.list.kind = kind;
  widget->as.list.margin = margin;

  glui->are_bounds_abs = false;
  glui->current_list = widget;
}

void glui_end_list(Glui *glui) {
  if (glui->current_list != glui->root_widget)
    glui->current_list = glui->current_list->parent;
}

void glui_text_id(Glui *glui, char *file_name,
                  u32 line, GluiWStr text, char *class) {
  GluiWidget *widget = glui_get_widget(glui->root_widget,
                                       glui->current_list,
                                       file_name, line);

  widget->kind = GluiWidgetKindText;
  widget->style = *glui_get_style(glui, class);
  widget->are_bounds_abs = glui->are_bounds_abs;
  if (widget->are_bounds_abs)
    widget->bounds = glui->current_abs_bounds;
  widget->parent = glui->current_list;

  widget->as.text.text = text;

  glui->are_bounds_abs = false;
}

GluiTextEditor *glui_text_editor_id(Glui *glui, char *file_name, u32 line,
                                    f32 text_size, char *class) {
  GluiWidget *widget = glui_get_widget(glui->root_widget,
                                       glui->current_list,
                                       file_name, line);

  widget->kind = GluiWidgetKindTextEditor;
  widget->style = *glui_get_style(glui, class);
  widget->are_bounds_abs = glui->are_bounds_abs;
  if (widget->are_bounds_abs)
    widget->bounds = glui->current_abs_bounds;
  widget->parent = glui->current_list;

  widget->as.text_editor.text_size = text_size;

  glui->are_bounds_abs = false;

  WinxEvent event = glui_get_event_of_kind(glui, EVENT_MASK(WinxEventKindKeyPress));
  if (event.kind == WinxEventKindKeyPress) {
    if (event.as.key_press.key_code == WinxKeyCodeLeft) {
      glui_text_editor_move_left(&widget->as.text_editor.editor);
    } else if (event.as.key_press.key_code == WinxKeyCodeRight) {
      glui_text_editor_move_right(&widget->as.text_editor.editor);
    } else if (event.as.key_press.key_code == WinxKeyCodeBackspace) {
      glui_text_editor_delete_prev(&widget->as.text_editor.editor);
    } else if (event.as.key_press.key_code == WinxKeyCodeDelete) {
      glui_text_editor_delete_next(&widget->as.text_editor.editor);
    } else if (event.as.key_press.key_code == WinxKeyCodeTab) {
      for (u32 i = 0; i < TEXT_EDITOR_TAB_WIDTH; ++i)
        glui_text_editor_insert(&widget->as.text_editor.editor, ' ');
    } else if (event.as.key_press._char) {
      glui_text_editor_insert(&widget->as.text_editor.editor, event.as.key_press._char);
    }
  }

  return &widget->as.text_editor.editor;
}
