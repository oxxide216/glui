#ifndef GLUI_VERTICES_H
#define GLUI_VERTICES_H

#include "glass/src/glass.h"
#include "glass/src/math.h"
#include "renderer.h"

typedef struct {
  Vec2 pos;
  Vec4 color;
} GluiGeneralVertex;

typedef Da(GluiGeneralVertex) GluiGeneralVertices;

typedef Da(u32) GluiIndices;

void glui_push_quad_vertices(GluiGeneralVertices *vertices,
                             GluiIndices *indices, Vec4 bounds,
                             Vec4 color);

#endif // GLUI_VERTICES_H
