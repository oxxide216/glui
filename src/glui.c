#include "glui.h"
#include "glass/src/params.h"
#include "vertices.h"

static Str general_vertex_shader_src = STR_LIT(
  "#version 330 core\n"
  "uniform vec2 u_resolution;\n"
  "layout (location = 0) in vec2 i_pos;\n"
  "void main(void) {\n"
  "  float x = i_pos.x / u_resolution.x * 2.0 - 1.0;\n"
  "  float y = 1.0 - i_pos.y / u_resolution.y * 2.0;\n"
  "  gl_Position = vec4(x, y, 0.0, 1.0);\n"
  "}\n"
);

static Str general_fragment_shader_src = STR_LIT(
  "#version 330 core\n"
  "uniform vec4 u_color;\n"
  "out vec4 frag_color;"
  "void main(void) {\n"
  "  frag_color = u_color;\n"
  "}\n"
);

Glui glui_init(u32 width, u32 height) {
  Glui glui = {0};

  GlassAttributes general_attributes = {0};
  glass_push_attribute(&general_attributes, GlassAttributeKindFloat, 2);
  glui.general_shader = glass_init_shader(general_vertex_shader_src,
                                          general_fragment_shader_src,
                                          &general_attributes);

  glui_resize(&glui, width, height);

  return glui;
}

static void glui_compute_layout(Glui *glui, u32 width, u32 height) {
  // Code here
}

void glui_resize(Glui *glui, u32 width, u32 height) {
  glui_compute_layout(glui, width, height);

  glass_set_param_2f(&glui->general_shader, "u_resolution",
                     vec2((f32) width, (f32) height));

  for (u32 i = 0; i < GLUI_PAIRS_CAP; ++i) {
    GluiPair *pair = glui->pairs[i];

    while (pair) {
      GluiWidget *widget = glui->widgets.items + pair->index;

      glass_set_param_4f(&glui->general_shader, "u_color", widget->color);

      switch (widget->kind) {
      case GluiWidgetKindButton: {
        Vec2 vertices[4];
        Vec2 *vertices_ptr = &*vertices;
        u32 vertices_size = glui_gen_quad_vertices(&vertices_ptr,
                                                   widget->pos,
                                                   widget->size);
        u32 indices[] = { 0, 1, 2, 2, 1, 3 };

        glass_put_object_data(&widget->as.button.body, vertices,
                              vertices_size, indices, sizeof(indices),
                              6, true);
      } break;
      }

      pair = pair->next;
    }
  }
}

void glui_render(Glui *glui) {
  for (u32 i = 0; i < glui->widgets.len; ++i) {
    GluiWidget *widget = glui->widgets.items + i;

    switch (widget->kind) {
    case GluiWidgetKindButton: {
      glass_render_object(&widget->as.button.body);
    } break;
    }
  }
}
