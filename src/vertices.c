#include "vertices.h"

void glui_push_quad_vertices(GluiGeneralVertices *vertices,
                             GluiIndices *indices, Vec4 bounds,
                             Vec4 color) {
  GluiGeneralVertex vertex0 = {
    vec2(bounds.x, bounds.y),
    color,
  };
  GluiGeneralVertex vertex1 = {
    vec2(bounds.x + bounds.z, bounds.y),
    color,
  };
  GluiGeneralVertex vertex2 = {
    vec2(bounds.x, bounds.y + bounds.w),
    color,
  };
  GluiGeneralVertex vertex3 = {
    vec2(bounds.x + bounds.z, bounds.y + bounds.w),
    color,
  };

  DA_APPEND(*indices, vertices->len);
  DA_APPEND(*indices, vertices->len + 1);
  DA_APPEND(*indices, vertices->len + 2);
  DA_APPEND(*indices, vertices->len + 2);
  DA_APPEND(*indices, vertices->len + 1);
  DA_APPEND(*indices, vertices->len + 3);

  DA_APPEND(*vertices, vertex0);
  DA_APPEND(*vertices, vertex1);
  DA_APPEND(*vertices, vertex2);
  DA_APPEND(*vertices, vertex3);
}
