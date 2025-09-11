#include <math.h>

#include "renderer.h"
#include "glass/src/params.h"
#include "vertices.h"
#include "shl_log.h"

#define TEXT_QUALITY_MULTIPLIER 2
#define TEXT_SIZE_MULTIPLIER    0.6
#define TEXT_Y_OFFSET           0.3

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

static Str texture_vertex_src = STR_LIT(
  "#version 330 core\n"
  "uniform vec2 u_resolution;\n"
  "layout (location = 0) in vec2 i_pos;\n"
  "layout (location = 1) in vec4 i_color;\n"
  "layout (location = 2) in vec2 i_uv;\n"
  "out vec4 o_color;\n"
  "out vec2 o_uv;\n"
  "void main(void) {\n"
  "  float x = i_pos.x / u_resolution.x * 2.0 - 1.0;\n"
  "  float y = 1.0 - i_pos.y / u_resolution.y * 2.0;\n"
  "  gl_Position = vec4(x, y, 0.0, 1.0);\n"
  "  o_color = i_color;\n"
  "  o_uv = i_uv;\n"
  "}\n"
);

static Str texture_fragment_src = STR_LIT(
  "#version 330 core\n"
  "uniform sampler2D u_texture;\n"
  "in vec4 o_color;\n"
  "in vec2 o_uv;\n"
  "out vec4 frag_color;\n"
  "void main(void) {\n"
  "  frag_color = vec4(1.0, 1.0, 1.0, texture(u_texture, o_uv).r) * o_color;\n"
  "}\n"
);

GluiRenderer glui_init_renderer(Vec2 size, char *font_file_path) {
  GluiRenderer renderer = {0};
  renderer.size = size;

  GlassAttributes general_attributes = {0};
  glass_push_attribute(&general_attributes, GlassAttributeKindFloat, 2);
  glass_push_attribute(&general_attributes, GlassAttributeKindFloat, 4);

  renderer.general_shader = glass_init_shader(general_vertex_src,
                                              general_fragment_src,
                                              &general_attributes);
  renderer.general_object = glass_init_object(&renderer.general_shader);

  GlassAttributes texture_attributes = {0};
  glass_push_attribute(&texture_attributes, GlassAttributeKindFloat, 2);
  glass_push_attribute(&texture_attributes, GlassAttributeKindFloat, 4);
  glass_push_attribute(&texture_attributes, GlassAttributeKindFloat, 2);

  renderer.texture_shader = glass_init_shader(texture_vertex_src,
                                              texture_fragment_src,
                                              &texture_attributes);

  FT_Init_FreeType(&renderer.freetype);


  if (FT_New_Face(renderer.freetype, font_file_path, 0, &renderer.face)) {
    ERROR("Could not load font from %s\n", font_file_path);
    exit(1);
  }

  return renderer;
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

static void glui_push_primitive(GluiRenderer *renderer, GluiPrimitiveKind kind,
                                Vec4 bounds, Vec4 color, u32 glyph_index) {
  GluiPrimitive primitive = { kind, bounds, color, glyph_index };

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

static u32 get_glyph_index(GluiRenderer *renderer, u32 _char, f32 text_size) {
  for (u32 i = 0; i < renderer->glyphs.len; ++i) {
    GluiGlyph *glyph = renderer->glyphs.items + i;

    if (glyph->_char == _char &&
        fabs(glyph->text_size - text_size) < 0.00001 &&
        !glyph->used) {
      glyph->used = true;
      return i;
    }
  }

  FT_Set_Pixel_Sizes(renderer->face, 0, text_size * TEXT_QUALITY_MULTIPLIER);
  FT_Load_Char(renderer->face, _char, FT_LOAD_RENDER);

  u32 advance = renderer->face->glyph->advance.x / TEXT_QUALITY_MULTIPLIER;
  UVec2 size = uvec2(renderer->face->glyph->bitmap.width / TEXT_QUALITY_MULTIPLIER,
                     renderer->face->glyph->bitmap.rows / TEXT_QUALITY_MULTIPLIER);
  UVec2 bearing = uvec2(renderer->face->glyph->bitmap_left / TEXT_QUALITY_MULTIPLIER,
                        renderer->face->glyph->bitmap_top / TEXT_QUALITY_MULTIPLIER);

  GlassObject object = glass_init_object(&renderer->texture_shader);

  GlassTextures textures = {0};
  glass_init_texture(&textures, renderer->face->glyph->bitmap.buffer,
                     renderer->face->glyph->bitmap.width,
                     renderer->face->glyph->bitmap.rows,
                     GlassPixelKindSingleColor,
                     GlassTextureFilteringModeLinear);

  GluiGlyph new_glyph = { _char, text_size, advance, size,
                          bearing, object, textures, true };
  DA_APPEND(renderer->glyphs, new_glyph);

  return renderer->glyphs.len - 1;
}

static void glui_gen_text_primitives(GluiRenderer *renderer,
                                     GluiWidget *widget,
                                     Str text, Vec4 bounds,
                                     Vec4 color) {
  f32 min_side_size = bounds.z < bounds.w ? bounds.z : bounds.w;
  f32 text_size = min_side_size * TEXT_SIZE_MULTIPLIER;

  f32 width = 0.0;

  for (u32 i = 0; i < text.len; ++i) {
    u32 _char = text.ptr[i];
    u32 glyph_index = get_glyph_index(renderer, _char, text_size);
    GluiGlyph *glyph = renderer->glyphs.items + glyph_index;

    if (i + 1 < widget->as.button.text.len)
      width += glyph->advance >> 6;
    else
      width += glyph->size.x;
  }

  f32 x_offset = (widget->bounds.z - width) / 2.0;

  for (u32 i = 0; i < text.len; ++i) {
    u32 _char = text.ptr[i];
    u32 glyph_index = get_glyph_index(renderer, _char, text_size);
    GluiGlyph *glyph = renderer->glyphs.items + glyph_index;
    f32 y_offset = (widget->bounds.w - glyph->size.y - glyph->bearing.y) / 2.0 +
                   TEXT_Y_OFFSET * text_size;

    Vec4 bounds = vec4(widget->bounds.x + x_offset,
                       widget->bounds.y + y_offset,
                       glyph->size.x,
                       glyph->size.y);

    glui_push_primitive(renderer, GluiPrimitiveKindTexture,
                        bounds, color, glyph_index);

    x_offset += glyph->advance >> 6;
  }
}

static void glui_gen_widget_primitives(GluiRenderer *renderer, GluiWidget *widget) {
  switch (widget->kind) {
  case GluiWidgetKindButton: {
    Vec4 bg_color, fg_color;
    if (widget->as.button.pressed) {
      bg_color = widget->style.alt_bg_color;
      fg_color = widget->style.alt_fg_color;
    } else {
      bg_color = widget->style.bg_color;
      fg_color = widget->style.fg_color;
    }

    glui_push_primitive(renderer, GluiPrimitiveKindQuad,
                        widget->bounds, bg_color, 0);

    glui_gen_text_primitives(renderer, widget,
                             widget->as.button.text,
                             widget->bounds, fg_color);
  } break;

  case GluiWidgetKindList: {
    glui_push_primitive(renderer, GluiPrimitiveKindQuad,
                        widget->bounds, widget->style.bg_color, 0);

    for (u32 i = 0; i < widget->as.list.children.len; ++i)
      glui_gen_widget_primitives(renderer, widget->as.list.children.items[i]);
  } break;

  case GluiWidgetKindText: {
    glui_gen_text_primitives(renderer, widget,
                             widget->as.text.text,
                             widget->bounds,
                             widget->style.fg_color);
  } break;
  }
}

static void glui_rerender(GluiRenderer *renderer) {
  GluiGeneralVertices general_vertices = {0};
  GluiIndices general_indices = {0};
  GluiTextureVertices texture_vertices = {0};
  GluiIndices texture_indices = {0};

  for (u32 i = 0; i < renderer->primitives.len; ++i) {
    GluiPrimitive *primitive = renderer->primitives.items + i;

    switch (primitive->kind) {
    case GluiPrimitiveKindQuad: {
      glui_push_quad_vertices(&general_vertices, &general_indices,
                              primitive->bounds, primitive->color);
    } break;

    case GluiPrimitiveKindTexture: {
      glui_push_texture_vertices(&texture_vertices, &texture_indices,
                                 primitive->bounds, primitive->color);

      GluiGlyph *glyph = renderer->glyphs.items + primitive->glyph_index;

      u32 texture_vertices_size = texture_vertices.len * sizeof(GluiTextureVertex);
      u32 texture_indices_size = texture_indices.len * sizeof(u32);

      glass_put_object_data(&glyph->object,
                            texture_vertices.items, texture_vertices_size,
                            texture_indices.items, texture_indices_size,
                            texture_indices.len, true);

      texture_vertices.len = 0;
      texture_indices.len = 0;
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

  if (texture_vertices.items) {
    free(texture_vertices.items);
    free(texture_indices.items);
  }

  glass_set_param_2f(&renderer->general_shader, "u_resolution", renderer->size);
  glass_set_param_2f(&renderer->texture_shader, "u_resolution", renderer->size);
}

void glui_render(GluiRenderer *renderer, GluiWidget *root_widget) {
  renderer->primitives.len = 0;
  for (u32 i = 0; i < renderer->glyphs.len; ++i)
    renderer->glyphs.items[i].used = false;

  glui_gen_widget_primitives(renderer, root_widget);

  if (renderer->redraw) {
    glui_rerender(renderer);

    renderer->redraw = false;
  }

  glass_render_object(&renderer->general_object, NULL);

  for (u32 i = 0; i < renderer->glyphs.len; ++i) {
    GluiGlyph *glyph = renderer->glyphs.items + i;

    glass_render_object(&glyph->object, &glyph->textures);
  }
}
