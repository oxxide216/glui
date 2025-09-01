#ifndef GLUI_H
#define GLUI_H

#include "winx/src/winx.h"
#include "winx/src/event.h"
#include "glass/src/math.h"
#include "shl_str.h"
#include "shl_defs.h"
#include "renderer.h"
#include "layout.h"

typedef enum {
  GluiWidgetKindButton = 0,
} GluiWidgetKind;

typedef struct {
  Str  text;
  bool pressed;
} GluiWidgetButton;

typedef union {
  GluiWidgetButton button;
} GluiWidgetAs;

typedef struct {
  Str  class;
  Vec4 bg_color;
  Vec4 fg_color;
} GluiStyle;

typedef Da(GluiStyle) GluiStyles;

typedef struct {
  GluiWidgetKind kind;
  GluiWidgetAs   as;
  GluiStyle      style;
  Vec4           bounds;
} GluiWidget;

typedef Da(GluiWidget) GluiWidgets;

typedef Da(WinxEvent) GluiEvents;

typedef struct {
  GluiWidgets   widgets;
  GluiStyles    styles;
  GluiRenderer  renderer;
  GluiLayout    layout;
  WinxWindow   *window;
  GluiEvents    events;
} Glui;

Glui      glui_init(WinxWindow *window);
WinxEvent glui_get_event(Glui *glui);
void      glui_next_frame(Glui *glui);

GluiStyle *glui_get_style(Glui *glui, Str class);

bool glui_button(Glui *glui, Str text, Vec4 color, Vec2 size);
void glui_begin_block(Glui *glui, Vec2 margin, GluiAnchor anchor,
                      Vec4 color, Vec2 size);
void glui_end_block(Glui *glui);

#endif // GLUI_H
