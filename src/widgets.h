#ifndef GLUI_WIDGETS_H
#define GLUI_WIDGETS_H

#include "glass/src/math.h"
#include "text-editor.h"
#include "wstr.h"

typedef enum {
  GluiWidgetKindButton = 0,
  GluiWidgetKindList,
  GluiWidgetKindText,
  GluiWidgetKindTextEditor,
} GluiWidgetKind;

typedef struct {
  GluiWStr text;
  bool     pressed;
} GluiWidgetButton;

typedef struct GluiWidget GluiWidget;

typedef Da(GluiWidget *) GluiWidgets;

typedef enum {
  GluiListKindVertical = 0,
  GluiListKindHorizontal,
} GluiListKind;

typedef struct {
  GluiListKind kind;
  GluiWidgets  children;
  Vec2         margin;
} GluiWidgetList;

typedef struct {
  GluiWStr text;
} GluiWidgetText;

typedef struct {
  GluiTextEditor editor;
  f32            text_size;
  bool           ctrl_pressed;
} GluiWidgetTextEditor;

typedef union {
  GluiWidgetButton     button;
  GluiWidgetList       list;
  GluiWidgetText       text;
  GluiWidgetTextEditor text_editor;
} GluiWidgetAs;

typedef struct {
  char *file_name;
  u32   line;
} GluiWidgetId;

typedef struct {
  char *class;
  Vec4  bg_color;
  Vec4  alt_bg_color;
  Vec4  fg_color;
  Vec4  alt_fg_color;
} GluiStyle;

typedef Da(GluiStyle) GluiStyles;

struct GluiWidget {
  GluiWidgetKind  kind;
  GluiWidgetAs    as;
  GluiWidgetId    id;
  GluiStyle       style;
  Vec4            bounds;
  bool            are_bounds_abs;
  bool            is_dirty;
  GluiWidget     *parent;
};

#endif // GLUI_WIDGETS_H
