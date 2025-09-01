#ifndef GLUI_PRIMITIVES_H
#define GLUI_PRIMITIVES_H

#include "glass/src/glass.h"
#include "glass/src/math.h"

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
  GluiPrimitives primitives;
  GluiPrimitives prev_primitives;
  GlassShader    general_shader;
  GlassObject    general_object;
  bool           redraw;
} GluiRenderer;

typedef struct {
  Vec2 pos;
  Vec4 color;
} GluiGeneralVertex;

typedef Da(GluiGeneralVertex) GluiGeneralVertices;

typedef Da(u32) GluiIndices;

GluiRenderer glui_init_renderer(Vec2 size);
void         glui_render(GluiRenderer *renderer);

void glui_push_quad(GluiRenderer *renderer, Vec4 bounds, Vec4 color);

#endif // GLUI_PRIMITIVES_H
