#ifndef GLUI_VERTICES_H
#define GLUI_VERTICES_H

#include "glass/glass.h"
#include "glass/math.h"
#include "renderer.h"

typedef struct {
  Vec2 pos;
  Vec4 color;
} GluiGeneralVertex;

typedef Da(GluiGeneralVertex) GluiGeneralVertices;

typedef struct {
  Vec2 pos;
  Vec4 color;
  Vec2 uv;
} GluiTextureVertex;

typedef Da(GluiTextureVertex) GluiTextureVertices;

typedef Da(u32) GluiIndices;

void glui_push_quad_vertices(GluiGeneralVertices *vertices,
                             GluiIndices *indices, Vec4 bounds,
                             Vec4 color);
void glui_push_texture_vertices(GluiTextureVertices *vertices,
                                GluiIndices *indices, Vec4 bounds,
                                Vec4 color, Vec4 uv);

#endif // GLUI_VERTICES_H
