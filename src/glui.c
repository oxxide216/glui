#include "glui/glui.h"
#include "winx/event.h"
#include "widgets.h"
#include "misc.h"
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

  DA_APPEND(glui->widgets, glui->current_list);
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
  if (!root_widget->is_dirty &&
      (!root_widget->is_visible ||
       (root_widget->kind == GluiWidgetKindList &&
        root_widget->as.list.prev_children_count == root_widget->as.list.children.len)))
    return;

  if (root_widget->kind == GluiWidgetKindList)
    root_widget->as.list.prev_children_count = root_widget->as.list.children.len;

  if (!root_widget->are_bounds_abs)
    root_widget->bounds = bounds;

  if (root_widget->kind != GluiWidgetKindList ||
      root_widget->as.list.children.len == 0)
    return;


  Vec4 free_space = vec4(root_widget->bounds.x + root_widget->as.list.margin.x,
                         root_widget->bounds.y + root_widget->as.list.margin.y,
                         root_widget->bounds.z - root_widget->as.list.margin.x * 2.0,
                         root_widget->bounds.w - root_widget->as.list.margin.y * 2.0);

  f32 child_width = 0.0;
  Vec2 offset = {0};

  if (root_widget->as.list.kind == GluiListKindHorizontal) {
    Vec4 temp_free_space = free_space;
    u32 fillers_count = 0;
    for (u32 i = 0; i < root_widget->as.list.children.len; ++i) {
      f32 width = root_widget->as.list.children.items[i]->fixed_width;

      if (width == 0.0) {
        ++fillers_count;
      } else {
        temp_free_space.z -= width;
        if (temp_free_space.z < 0.0)
          temp_free_space.z = 0.0;
      }
    }

    child_width = (temp_free_space.z - root_widget->as.list.margin.x *
                   (root_widget->as.list.children.len - 1.0)) /
                   fillers_count;
    free_space.z = child_width;
    offset.x = child_width + root_widget->as.list.margin.x;
  } else {
    Vec4 temp_free_space = free_space;
    u32 fillers_count = 0;
    for (u32 i = 0; i < root_widget->as.list.children.len; ++i) {
      f32 width = root_widget->as.list.children.items[i]->fixed_width;

      if (width == 0.0) {
        ++fillers_count;
      } else {
        temp_free_space.w -= width;
        if (temp_free_space.w < 0.0)
          temp_free_space.w = 0.0;
      }
    }

    child_width = (temp_free_space.w - root_widget->as.list.margin.y *
                   (root_widget->as.list.children.len - 1.0)) /
                   fillers_count;
    free_space.w = child_width;
    offset.y = free_space.w + root_widget->as.list.margin.y;
  }

  for (u32 i = 0; i < root_widget->as.list.children.len; ++i) {
    GluiWidget *child = root_widget->as.list.children.items[i];
    Vec4 child_bounds = free_space;
    if (child->fixed_width == 0.0) {
      if (child->as.list.kind == GluiListKindHorizontal)
        child_bounds.z = child_width;
      else
        child_bounds.w = child_width;
    } else {
      if (child->as.list.kind == GluiListKindHorizontal)
        child_bounds.z = child->fixed_width;
      else
        child_bounds.w = child->fixed_width;
    }

    glui_compute_bounds(child, child_bounds);

    if (child->fixed_width == 0.0) {
      if (child->as.list.kind == GluiListKindHorizontal)
        free_space.x += offset.x;
      else
        free_space.y += offset.y;
    } else {
      if (child->as.list.kind == GluiListKindHorizontal)
        free_space.x += child->fixed_width + root_widget->as.list.margin.x;
      else
        free_space.y += child->fixed_width + root_widget->as.list.margin.y;
    }
  }
}

static void glui_reset_widget(GluiWidget *widget, bool is_dirty) {
  widget->is_dirty = is_dirty;
  widget->is_visible = false;

  if (widget->kind != GluiWidgetKindList)
    return;

  for (u32 i = 0; i < widget->as.list.children.len; ++i)
    glui_reset_widget(widget->as.list.children.items[i], is_dirty);

  widget->as.list.prev_children_count = widget->as.list.children.len;
  widget->as.list.children.len = 0;
}

void glui_next_frame(Glui *glui) {
  glui->current_list = glui->root_widget;
  glui->current_list->style = *glui_get_style(glui, "root");

  Vec4 root_widget_bounds = vec4(0.0, 0.0, glui->size.x, glui->size.y);
  glui_compute_bounds(glui->root_widget, root_widget_bounds);
  glui_render(&glui->renderer, glui->root_widget);

  bool layout_is_dirty = false;

  if (glui->renderer.size.x != (f32) glui->window->width) {
    glui->size.x = (f32) glui->window->width;
    glui->renderer.size.x = (f32) glui->window->width;
    glui->root_widget->is_dirty = true;
    layout_is_dirty = true;
  }

  if (glui->renderer.size.y != (f32) glui->window->height) {
    glui->size.y = (f32) glui->window->height;
    glui->renderer.size.y = (f32) glui->window->height;
    glui->root_widget->is_dirty = true;
    layout_is_dirty = true;
  }

  glui_reset_widget(glui->root_widget, layout_is_dirty);
  glui->root_widget->is_visible = true;

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

void glui_fixed_width(Glui *glui, f32 width) {
  glui->fixed_width = width;
}

static void glui_mark_widget_as_dirty(GluiWidget *widget) {
  widget->is_dirty = true;

  if (widget->parent)
    glui_mark_widget_as_dirty(widget->parent);
}

static GluiWidget *glui_get_widget(Glui *glui, char *file_name, u32 line) {
  GluiWidget *widget = NULL;

  for (u32 i = 0; i < glui->widgets.len; ++i) {
    GluiWidget *temp_widget = glui->widgets.items[i];
    if (strcmp(temp_widget->id.file_name, file_name) == 0 &&
        temp_widget->id.line == line) {
      widget = temp_widget;
      break;
    }
  }

  if (widget)
    return widget;

  widget = aalloc(sizeof(GluiWidget));
  *widget = (GluiWidget) {0};
  widget->id.file_name = file_name;
  widget->id.line = line;
  widget->is_dirty = true;

  DA_APPEND(glui->widgets, widget);

  return widget;
}

GluiWidget *glui_setup_widget(Glui *glui, GluiWidgetKind kind,
                              char *file_name, u32 line, char *class) {
  GluiWidget *widget = glui_get_widget(glui, file_name, line);
  GluiStyle *style = glui_get_style(glui, class);

  widget->parent = glui->current_list;

  if (widget->are_bounds_abs != glui->are_bounds_abs ||
      widget->fixed_width != glui->fixed_width ||
      widget->is_dirty ||
      (glui->are_bounds_abs &&
       !glui_vec4_eq(&widget->bounds, &glui->current_abs_bounds)))
    glui_mark_widget_as_dirty(widget);

  widget->kind = kind;
  widget->style = *style;
  widget->are_bounds_abs = glui->are_bounds_abs;
  if (widget->are_bounds_abs) {
    widget->bounds = glui->current_abs_bounds;
  } else if (glui->fixed_width != 0.0) {
    widget->fixed_width = glui->fixed_width;
    glui->fixed_width = 0.0;
  }
  widget->is_visible = true;

  DA_APPEND(glui->current_list->as.list.children, widget);

  return widget;
}

bool glui_button_id(Glui *glui, char *file_name, u32 line,
                    GluiWStr text, char *class) {
  GluiWidget *widget = glui_setup_widget(glui, GluiWidgetKindButton,
                                         file_name, line, class);

  widget->as.button.text = text;

  glui->are_bounds_abs = false;

  for (u32 i = 0; i < glui->events.len; ++i) {
    WinxEvent *event = glui->events.items + i;
    if (event->was_processed)
      continue;

    if (event->kind == WinxEventKindButtonPress) {
      f32 x = (f32) event->as.button.x;
      f32 y = (f32) event->as.button.y;

      if (x >= widget->bounds.x && x <= widget->bounds.x + widget->bounds.z &&
          y >= widget->bounds.y && y <= widget->bounds.y + widget->bounds.w)
        widget->as.button.pressed = true;
    } else if (widget->as.button.pressed &&
               event->kind == WinxEventKindButtonRelease) {
      widget->as.button.pressed = false;

      f32 x = (f32) event->as.button.x;
      f32 y = (f32) event->as.button.y;

      if (x >= widget->bounds.x && x <= widget->bounds.x + widget->bounds.z &&
          y >= widget->bounds.y && y <= widget->bounds.y + widget->bounds.w)
        return true;
    }
  }

  return false;
}

void glui_begin_list_id(Glui *glui, char *file_name, u32 line,
                        GluiListKind kind, Vec2 margin, char *class) {
  GluiWidget *widget = glui_setup_widget(glui, GluiWidgetKindList,
                                         file_name, line, class);

  if (widget->as.list.kind != kind ||
      !glui_vec2_eq(&widget->as.list.margin, &margin))
    glui_mark_widget_as_dirty(widget);

  widget->as.list.kind = kind;
  widget->as.list.margin = margin;

  glui->are_bounds_abs = false;
  glui->current_list = widget;
}

void glui_end_list(Glui *glui) {
  if (glui->current_list != glui->root_widget)
    glui->current_list = glui->current_list->parent;
}

void glui_text_id(Glui *glui, char *file_name, u32 line,
                  GluiWStr text, bool center, char *class) {
  GluiWidget *widget = glui_setup_widget(glui, GluiWidgetKindText,
                                         file_name, line, class);

  widget->as.text.text = text;
  widget->as.text.center = center;

  glui->are_bounds_abs = false;
}

GluiTextEditor *glui_text_editor_id(Glui *glui, char *file_name, u32 line,
                                    f32 text_size, Vec2 scroll_speed, char *class) {
  GluiWidget *widget = glui_setup_widget(glui, GluiWidgetKindTextEditor,
                                         file_name, line, class);

  if (widget->as.text_editor.editor.lines.len == 0)
    DA_APPEND(widget->as.text_editor.editor.lines, (GluiTextEditorLine) {0});
  widget->as.text_editor.text_size = text_size;

  glui->are_bounds_abs = false;

  if (widget->as.text_editor.editor.is_locked)
    return &widget->as.text_editor.editor;

  for (u32 i = 0; i < glui->events.len; ++i) {
    WinxEvent *event = glui->events.items + i;
    if (event->was_processed)
      continue;

    if (event->kind == WinxEventKindKeyRelease) {
      if (event->as.key.key_code == WinxKeyCodeLeftControl)
        widget->as.text_editor.ctrl_pressed = false;
    } else if (event->kind == WinxEventKindKeyPress ||
               event->kind == WinxEventKindKeyHold) {
      WinxKeyCode key_code = event->as.key.key_code;
      GluiWChar _char = event->as.key._char;

      bool new_is_dirty = false;
      u32 prev_row = widget->as.text_editor.editor.row;

      switch (key_code) {
      case WinxKeyCodeLeftControl: {
        if (event->kind == WinxEventKindKeyPress)
          widget->as.text_editor.ctrl_pressed = true;
      } break;

      case WinxKeyCodeLeft: {
        if (widget->as.text_editor.ctrl_pressed)
          glui_text_editor_move_left_word(&widget->as.text_editor.editor, false);
        else
          glui_text_editor_move_left(&widget->as.text_editor.editor);
      } break;

      case WinxKeyCodeRight: {
        if (widget->as.text_editor.ctrl_pressed)
          glui_text_editor_move_right_word(&widget->as.text_editor.editor, false);
        else
          glui_text_editor_move_right(&widget->as.text_editor.editor);
      } break;

      case WinxKeyCodeDown: {
        if (widget->as.text_editor.ctrl_pressed)
          glui_text_editor_move_down_paragraph(&widget->as.text_editor.editor);
        else
          glui_text_editor_move_down(&widget->as.text_editor.editor);
      } break;

      case WinxKeyCodeUp: {
        if (widget->as.text_editor.ctrl_pressed)
          glui_text_editor_move_up_paragraph(&widget->as.text_editor.editor);
        else
          glui_text_editor_move_up(&widget->as.text_editor.editor);
      } break;

      case WinxKeyCodeBackspace: {
        if (widget->as.text_editor.ctrl_pressed)
          glui_text_editor_move_left_word(&widget->as.text_editor.editor, true);
        else
          glui_text_editor_delete_prev(&widget->as.text_editor.editor);

        new_is_dirty = true;
      } break;

      case WinxKeyCodeDelete: {
        if (widget->as.text_editor.ctrl_pressed)
          glui_text_editor_move_right_word(&widget->as.text_editor.editor, true);
        else
          glui_text_editor_delete_next(&widget->as.text_editor.editor);

        new_is_dirty = true;
      } break;

      case WinxKeyCodeTab: {
        if (!widget->as.text_editor.ctrl_pressed) {
          for (u32 i = 0; i < TEXT_EDITOR_TAB_WIDTH; ++i)
            glui_text_editor_insert(&widget->as.text_editor.editor, ' ');
          new_is_dirty = true;
        }
      } break;

      case WinxKeyCodeEnter: {
        if (!widget->as.text_editor.ctrl_pressed) {
          glui_text_editor_insert(&widget->as.text_editor.editor, '\n');
          new_is_dirty = true;
        }
      } break;

      default: {
        if (!widget->as.text_editor.ctrl_pressed && _char) {
          glui_text_editor_insert(&widget->as.text_editor.editor, _char);
          new_is_dirty = true;
        }
      } break;
      }

      widget->is_dirty |= new_is_dirty;

      f32 text_size_scaled = widget->as.text_editor.text_size * TEXT_SIZE_MULTIPLIER;

      if (widget->as.text_editor.editor.row > prev_row &&
          (widget->as.text_editor.editor.row + 1) * text_size_scaled >
          widget->bounds.w)
        widget->as.text_editor.scroll.y += (widget->as.text_editor.editor.row - prev_row) *
                                           text_size_scaled;
      else if (widget->as.text_editor.editor.row * text_size_scaled <
               widget->as.text_editor.scroll.y - glui->renderer.font_descent)
        widget->as.text_editor.scroll.y -= (prev_row - widget->as.text_editor.editor.row) *
                                           text_size_scaled;

    } else if (event->kind == WinxEventKindButtonPress) {
      if (event->as.button.button == WinxMouseButtonWheelUp)
        widget->as.text_editor.scroll.y += scroll_speed.y;
      if (event->as.button.button == WinxMouseButtonWheelDown)
        widget->as.text_editor.scroll.y -= scroll_speed.y;
    }

    if (widget->as.text_editor.scroll.y < 0.0)
      widget->as.text_editor.scroll.y = 0.0;
  }

  return &widget->as.text_editor.editor;
}
