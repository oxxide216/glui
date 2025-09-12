#ifndef GLUI_H
#define GLUI_H

#include "winx/src/winx.h"
#include "winx/src/event.h"
#include "glass/src/math.h"
#include "shl_str.h"
#include "shl_defs.h"
#include "widgets.h"
#include "renderer.h"
#include "text-editor.h"

#define glui_button(glui, text, class)                  \
  glui_button_id(glui, __FILE__, __LINE__, text, class)

#define glui_begin_list(glui, kind, margin, class)                  \
  glui_begin_list_id(glui, __FILE__, __LINE__, kind, margin, class)

#define glui_text(glui, text, class)                  \
  glui_text_id(glui, __FILE__, __LINE__, text, class)

#define glui_text_editor(glui, class)                  \
  glui_text_editor_id(glui, __FILE__, __LINE__, class)

typedef Da(WinxEvent) GluiEvents;

typedef struct {
  Vec2          size;
  GluiWidget   *root_widget;
  GluiWidget   *current_list;
  GluiStyles    styles;
  GluiRenderer  renderer;
  WinxWindow   *window;
  GluiEvents    events;
  Vec4          current_abs_bounds;
  bool          are_bounds_abs;
} Glui;

Glui      glui_init(WinxWindow *window, char *font_file_path);
WinxEvent glui_get_event(Glui *glui);
void      glui_next_frame(Glui *glui);

GluiStyle *glui_get_style(Glui *glui, char *class);
void       glui_abs_bounds(Glui *glui, Vec4 bounds);

bool            glui_button_id(Glui *glui, char *file_name, u32 line, Str text, char *class);
void            glui_begin_list_id(Glui *glui, char *file_name, u32 line,
                               GluiListKind kind, Vec2 margin, char *class);
void            glui_end_list(Glui *glui);
void            glui_text_id(Glui *glui, char *file_name, u32 line, Str text, char *class);
GluiTextEditor *glui_text_editor_id(Glui *glui, char *file_name, u32 line, char *class);

#endif // GLUI_H
