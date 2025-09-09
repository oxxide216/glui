#ifndef GLUI_WIDGETS_H
#define GLUI_WIDGETS_H

#include "glass/src/math.h"

typedef enum {
  GluiWidgetKindButton = 0,
  GluiWidgetKindList,
} GluiWidgetKind;

typedef struct {
  Str  text;
  bool pressed;
} GluiWidgetButton;

typedef struct GluiWidget GluiWidget;

typedef Da(GluiWidget *) GluiWidgets;

typedef enum {
  GluiAnchorTopLeft = 0,
  GluiAnchorTopRight,
  GluiAnchorBottomLeft,
  GluiAnchorBottomRight,
} GluiAnchor;

typedef struct {
  GluiWidgets children;
  Vec2        margin;
  Vec2        free_space_pos;
  Vec2        free_space_size;
  GluiAnchor  anchor;
  bool        fill_x;
  bool        fill_y;
} GluiWidgetList;

typedef union {
  GluiWidgetButton button;
  GluiWidgetList   list;
} GluiWidgetAs;

typedef struct {
  char *file_name;
  u32   line;
} GluiWidgetId;

typedef struct {
  char *class;
  Vec4  bg_color;
  Vec4  fg_color;
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
  GluiWidget     *parent;
};

#endif // GLUI_WIDGETS_H
