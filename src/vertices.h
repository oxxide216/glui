#ifndef GLUI_VERTICES_H
#define GLUI_VERTICES_H

#include "glass/src/glass.h"
#include "glass/src/math.h"
#include "renderer.h"

void glui_push_quad_vertices(GluiGeneralVertices *vertices,
                             GluiIndices *indices, Vec4 bounds,
                             Vec4 color);

#endif // GLUI_VERTICES_H
