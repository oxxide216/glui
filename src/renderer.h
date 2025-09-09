#ifndef GLUI_PRIMITIVES_H
#define GLUI_PRIMITIVES_H

#include "glass/src/glass.h"
#include "glass/src/math.h"
#include "widgets.h"

typedef enum {
  GluiPrimitiveKindQuad = 0,
} GluiPrimitiveKind;

typedef struct {
  GluiPrimitiveKind kind;
  Vec4              bounds;
  Vec4              color;
} GluiPrimitive;

typedef Da(GluiPrimitive) GluiPrimitives;

typedef struct {
  Vec2           size;
  bool           redraw;
  GluiPrimitives primitives;
  GluiPrimitives prev_primitives;
  GlassShader    general_shader;
  GlassObject    general_object;
} GluiRenderer;

GluiRenderer glui_init_renderer(Vec2 size);
void         glui_render(GluiRenderer *renderer, GluiWidget *root_widget);

#endif // GLUI_PRIMITIVES_H
