#ifndef GLUI_WIDGETS_H
#define GLUI_WIDGETS_H

#include "glass/src/math.h"

typedef enum {
  GluiWidgetKindButton = 0,
  GluiWidgetKindList,
  GluiWidgetKindText,
} GluiWidgetKind;

typedef struct {
  Str  text;
  bool pressed;
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
  Str  text;
} GluiWidgetText;

typedef union {
  GluiWidgetButton button;
  GluiWidgetList   list;
  GluiWidgetText   text;
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

typedef enum {
  GluiAnchorXLeft = 0,
  GluiAnchorXRight,
} GluiAnchorX;

typedef enum {
  GluiAnchorYTop = 0,
  GluiAnchorYBottom,
} GluiAnchorY;

struct GluiWidget {
  GluiWidgetKind  kind;
  GluiWidgetAs    as;
  GluiWidgetId    id;
  GluiStyle       style;
  Vec4            bounds;
  bool            are_bounds_abs;
  GluiWidget     *parent;
};

#endif // GLUI_WIDGETS_H
