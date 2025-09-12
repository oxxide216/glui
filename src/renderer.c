#include <math.h>

#include "renderer.h"
#include "glass/src/params.h"
#include "vertices.h"
#include "shl_log.h"

#define TEXT_QUALITY_MULTIPLIER 2.0
#define TEXT_SIZE_MULTIPLIER    0.6
#define LINE_SPACING            0.0

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
  renderer.texture_object = glass_init_object(&renderer.texture_shader);
  renderer.glyphs_texture = glass_init_texture(GlassFilteringModeLinear);

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
                                Vec4 bounds, Vec4 color, Vec4 uv) {
  GluiPrimitive primitive = { kind, bounds, color, uv };

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

static u8 *glui_concat_texture_buffers(u32 *new_width, u32 *new_height,
                                       u8 *a, u8 *b,
                                       u32 a_width, u32 a_height,
                                       u32 b_width, u32 b_height) {
  *new_width = a_width + b_width;
  *new_height = a_height > b_height ? a_height : b_height;
  u8 *new_buffer = calloc(*new_width * *new_height, sizeof(u8));

  for (u32 y = 0; y < a_height; ++y)
    for (u32 x = 0; x < a_width; ++x)
      new_buffer[y * *new_width + x] = a[y * a_width + x];

  for (u32 y = 0; y < b_height; ++y)
    for (u32 x = 0; x < b_width; ++x)
      new_buffer[y * *new_width + x + a_width] = b[y * b_width + x];

  return new_buffer;
}

static u32 get_glyph_index(GluiRenderer *renderer, u32 _char, f32 text_size) {
  for (u32 i = 0; i < renderer->glyphs.len; ++i) {
    GluiGlyph *glyph = renderer->glyphs.items + i;

    if (glyph->_char == _char && fabs(glyph->text_size - text_size) < 0.00001)
      return i;
  }

  FT_Set_Pixel_Sizes(renderer->face, 0, text_size * TEXT_QUALITY_MULTIPLIER);
  FT_Load_Char(renderer->face, _char, FT_LOAD_RENDER);

  Vec2 size = vec2(renderer->face->glyph->bitmap.width / TEXT_QUALITY_MULTIPLIER,
                   renderer->face->glyph->bitmap.rows / TEXT_QUALITY_MULTIPLIER);

  Vec2 bearing = vec2(renderer->face->glyph->bitmap_left / TEXT_QUALITY_MULTIPLIER,
                      renderer->face->glyph->bitmap_top / TEXT_QUALITY_MULTIPLIER);

  u32 advance = renderer->face->glyph->advance.x / TEXT_QUALITY_MULTIPLIER;

  f32 uv_x_pos = renderer->glyphs_texture_width;

  u8 *new_glyphs_texture_buffer =
    glui_concat_texture_buffers(&renderer->glyphs_texture_width,
                                &renderer->glyphs_texture_height,
                                renderer->glyphs_texture_buffer,
                                renderer->face->glyph->bitmap.buffer,
                                renderer->glyphs_texture_width,
                                renderer->glyphs_texture_height,
                                renderer->face->glyph->bitmap.width,
                                renderer->face->glyph->bitmap.rows);

  free(renderer->glyphs_texture_buffer);
  renderer->glyphs_texture_buffer = new_glyphs_texture_buffer;

  glass_put_texture_data(&renderer->glyphs_texture,
                         renderer->glyphs_texture_buffer,
                         renderer->glyphs_texture_width,
                         renderer->glyphs_texture_height,
                         GlassPixelKindSingleColor);

  GluiGlyph new_glyph = { _char, text_size, uv_x_pos, size, bearing, advance };
  DA_APPEND(renderer->glyphs, new_glyph);

  return renderer->glyphs.len - 1;
}

static f32 glui_calculate_x_offset(GluiRenderer *renderer,
                                   GluiWidget *widget,
                                   Str text, f32 text_size) {
  f32 max_width = 0.0;
  f32 width = 0.0;

  for (u32 i = 0; i < text.len; ++i) {
    u32 _char = text.ptr[i];
    if (_char == '\n') {
      if (max_width < width)
        max_width = width;
      width = 0.0;
      continue;
    }

    u32 glyph_index = get_glyph_index(renderer, _char, text_size);
    GluiGlyph *glyph = renderer->glyphs.items + glyph_index;

    if (i + 1 < text.len)
      width += glyph->advance >> 6;
    else
      width += glyph->size.x;
  }

  if (max_width < width)
    max_width = width;

  return (widget->bounds.z - max_width) / 2.0;
}

static void glui_gen_text_primitives(GluiRenderer *renderer,
                                     GluiWidget *widget,
                                     Str text, Vec4 bounds,
                                     bool center, Vec4 color) {
  f32 min_side_size = bounds.z < bounds.w ? bounds.z : bounds.w;
  f32 text_size = min_side_size * TEXT_SIZE_MULTIPLIER;
  f32 base_x_offset = 0.0;
  if (center)
    base_x_offset = glui_calculate_x_offset(renderer, widget, text, text_size);
  f32 x_offset = base_x_offset;
  u32 line_index = 0;

  for (u32 i = 0; i < text.len; ++i) {
    u32 _char = text.ptr[i];
    if (_char == '\n') {
      x_offset = base_x_offset;
      ++line_index;
      continue;
    }

    u32 glyph_index = get_glyph_index(renderer, _char, text_size);
    GluiGlyph *glyph = renderer->glyphs.items + glyph_index;
    f32 y_offset = glyph->bearing.y + (line_index + LINE_SPACING) * text_size;

    Vec4 glyph_bounds = vec4(bounds.x + x_offset + glyph->bearing.x,
                             bounds.y + y_offset,
                             glyph->size.x,
                             glyph->size.y);

    Vec4 uv = vec4(glyph->uv_x_pos / renderer->glyphs_texture_width,
                   0.0,
                   (glyph->uv_x_pos + glyph->size.x * TEXT_QUALITY_MULTIPLIER) /
                   renderer->glyphs_texture_width,
                   glyph->size.y * TEXT_QUALITY_MULTIPLIER /
                   renderer->glyphs_texture_height);

    glui_push_primitive(renderer, GluiPrimitiveKindTexture,
                        glyph_bounds, color, uv);

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
                        widget->bounds, bg_color, (Vec4) {0});

    glui_gen_text_primitives(renderer, widget,
                             widget->as.button.text,
                             widget->bounds, true, fg_color);
  } break;

  case GluiWidgetKindList: {
    glui_push_primitive(renderer, GluiPrimitiveKindQuad,
                        widget->bounds, widget->style.bg_color, (Vec4) {0});

    for (u32 i = 0; i < widget->as.list.children.len; ++i)
      glui_gen_widget_primitives(renderer, widget->as.list.children.items[i]);
  } break;

  case GluiWidgetKindText: {
    glui_gen_text_primitives(renderer, widget,
                             widget->as.text.text,
                             widget->bounds,
                             true,
                             widget->style.fg_color);
  } break;

  case GluiWidgetKindTextEditor: {
    glui_push_primitive(renderer, GluiPrimitiveKindQuad,
                        widget->bounds, widget->style.bg_color, (Vec4) {0});

    Str text = STR(
      (char *) widget->as.text_editor.editor.buffer.items,
      widget->as.text_editor.editor.buffer.len * sizeof(u32)
    );

    Vec4 text_bounds = widget->bounds;
    text_bounds.w = 48.0;

    glui_gen_text_primitives(renderer, widget, text, text_bounds,
                             false, widget->style.fg_color);
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
                                 primitive->bounds, primitive->color,
                                 primitive->uv);
    } break;
    }
  }

  u32 general_vertices_size = general_vertices.len * sizeof(GluiGeneralVertex);
  u32 general_indices_size = general_indices.len * sizeof(u32);
  u32 texture_vertices_size = texture_vertices.len * sizeof(GluiTextureVertex);
  u32 texture_indices_size = texture_indices.len * sizeof(u32);

  glass_put_object_data(&renderer->general_object,
                        general_vertices.items, general_vertices_size,
                        general_indices.items, general_indices_size,
                        general_indices.len, true);

  glass_put_object_data(&renderer->texture_object,
                        texture_vertices.items, texture_vertices_size,
                        texture_indices.items, texture_indices_size,
                        texture_indices.len, true);

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
  glui_gen_widget_primitives(renderer, root_widget);

  if (renderer->redraw) {
    glui_rerender(renderer);

    renderer->redraw = false;
  }

  glass_render_object(&renderer->general_object, NULL, 0);
  glass_render_object(&renderer->texture_object, &renderer->glyphs_texture, 1);
}
