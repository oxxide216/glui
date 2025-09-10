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

void glui_push_texture_vertices(GluiTextureVertices *vertices,
                                GluiIndices *indices, Vec4 bounds,
                                Vec4 color) {
  GluiTextureVertex vertex0 = {
    vec2(bounds.x, bounds.y),
    color, vec2(0.0, 0.0),
  };
  GluiTextureVertex vertex1 = {
    vec2(bounds.x + bounds.z, bounds.y),
    color, vec2(1.0, 0.0),
  };
  GluiTextureVertex vertex2 = {
    vec2(bounds.x, bounds.y + bounds.w),
    color, vec2(0.0, 1.0),
  };
  GluiTextureVertex vertex3 = {
    vec2(bounds.x + bounds.z, bounds.y + bounds.w),
    color, vec2(1.0, 1.0),
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
