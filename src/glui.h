#ifndef GLUI_H
#define GLUI_H

#include "glass/src/glass.h"
#include "glass/src/math.h"

#define GLUI_PAIRS_CAP 10

typedef enum {
  GluiWidgetKindButton = 0,
} GluiWidgetKind;

typedef struct {
  Str         text;
  GlassObject body;
} GluiWidgetButton;

typedef union {
  GluiWidgetButton button;
} GluiWidgetAs;

typedef enum {
  GluiEventNone = 0,
  GluiEventHover = 1 << 0,
  GluiEventLeave = 1 << 1,
  GluiEventPress = 1 << 2,
  GluiEventRelease = 1 << 3,
} GluiEvent;

typedef enum {
  GluiAnchorTopLeft = 0,
  GluiAnchorTopRight,
  GluiAnchorBottomLeft,
  GluiAnchorBottomRight,
} GluiAnchor;

typedef enum {
  GluiMarginTop = 0,
  GluiMarginBottom,
  GluiMarginLeft,
  GluiMarginRight,
} GluiMargin;

typedef struct {
  GluiEvent      *event;
  GluiAnchor      anchor;
  float           margin_values[4];
} GluiWidgetConfig;

typedef struct {
  GluiWidgetConfig config;
  GluiWidgetKind   kind;
  GluiWidgetAs     as;
  Vec2             pos;
  Vec2             size;
  Vec4             color;
} GluiWidget;

typedef Da(GluiWidget) GluiWidgets;

typedef struct GluiPair GluiPair;

struct GluiPair {
  u32       index;
  GluiPair *next;
};

typedef struct {
  GluiWidgets  widgets;
  GluiPair    *pairs[GLUI_PAIRS_CAP];
  GlassShader  general_shader;
} Glui;

Glui glui_init(u32 width, u32 height);
void glui_resize(Glui *glui, u32 width, u32 height);
void glui_render(Glui *glui);

#endif // GLUI_H
