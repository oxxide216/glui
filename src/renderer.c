#include "renderer.h"
#include "glass/src/params.h"
#include "vertices.h"
#include "shl_log.h"

static Str general_vertex_src = STR_LIT(
  "#version 330 core\n"
  "uniform vec2 u_resolution;\n"
  "layout (location = 0) in vec2 i_pos;\n"
  "layout (location = 1) in vec4 i_color;\n"
  "out vec4 o_color;\n"
  "void main(void) {\n"
  "  float x = i_pos.x / u_resolution.x * 2.0 - 1.0;\n"
  "  float y = 1.0 - i_pos.y / u_resolution.y * 2.0;\n"
  "  gl_Position = vec4(x, y, 0.0, 1.0);\n"
  "  o_color = i_color;\n"
  "}\n"
);

static Str general_fragment_src = STR_LIT(
  "#version 330 core\n"
  "in vec4 o_color;\n"
  "out vec4 frag_color;\n"
  "void main(void) {\n"
  "  frag_color = o_color;\n"
  "}\n"
);

GluiRenderer glui_init_renderer(Vec2 size) {
  GluiRenderer renderer = {0};
  renderer.size = size;

  GlassAttributes general_attributes = {0};
  glass_push_attribute(&general_attributes, GlassAttributeKindFloat, 2);
  glass_push_attribute(&general_attributes, GlassAttributeKindFloat, 4);

  renderer.general_shader = glass_init_shader(general_vertex_src,
                                              general_fragment_src,
                                              &general_attributes);
  renderer.general_object = glass_init_object(&renderer.general_shader);

  return renderer;
}

static void glui_redraw(GluiRenderer *renderer) {
  GluiGeneralVertices general_vertices = {0};
  GluiIndices general_indices = {0};

  for (u32 i = 0; i < renderer->primitives.len; ++i) {
    GluiPrimitive *primitive = renderer->primitives.items + i;

    switch (primitive->kind) {
    case GluiPrimitiveKindQuad: {
      glui_push_quad_vertices(&general_vertices, &general_indices,
                              primitive->bounds, primitive->color);
    } break;
    }
  }

  u32 general_vertices_size = general_vertices.len * sizeof(GluiGeneralVertex);
  u32 general_indices_size = general_indices.len * sizeof(u32);

  glass_put_object_data(&renderer->general_object,
                        general_vertices.items, general_vertices_size,
                        general_indices.items, general_indices_size,
                        general_indices.len, true);

  if (general_vertices.items) {
    free(general_vertices.items);
    free(general_indices.items);
  }

  glass_set_param_2f(&renderer->general_shader, "u_resolution", renderer->size);
}

void glui_render(GluiRenderer *renderer) {
  if (renderer->redraw) {
    renderer->redraw = false;

    glui_redraw(renderer);
  }

  if (renderer->prev_primitives.items)
    free(renderer->prev_primitives.items);

  renderer->prev_primitives = renderer->primitives;
  renderer->primitives = (GluiPrimitives) {0};

  glass_render_object(&renderer->general_object);
}

static bool glui_vec4_eq(Vec4 *a, Vec4 *b) {
  return a->x == b->x && a->y == b->y &&
         a->z == b->z && a->w == b->w;
}

static bool glui_primitive_eq(GluiPrimitive *a, GluiPrimitive *b) {
  if (a->kind != b->kind)
    return false;

  if (!glui_vec4_eq(&a->bounds, &b->bounds))
    return false;

  if (!glui_vec4_eq(&a->color, &b->color))
    return false;

  return true;
}

static void glui_push_primitive(GluiRenderer *renderer, GluiPrimitive primitive) {
  if (renderer->primitives.len >= renderer->prev_primitives.len) {
    renderer->redraw = true;
  } else {
    GluiPrimitive *prev_primitive = renderer->prev_primitives.items +
                                    renderer->primitives.len;
    if (!glui_primitive_eq(prev_primitive, &primitive))
      renderer->redraw = true;
  }

  DA_APPEND(renderer->primitives, primitive);
}

void glui_push_quad(GluiRenderer *renderer, Vec4 bounds, Vec4 color) {
  GluiPrimitive quad = { GluiPrimitiveKindQuad, bounds, color };
  glui_push_primitive(renderer, quad);
}
