#ifndef GLUI_PRIMITIVES_H
#define GLUI_PRIMITIVES_H

#include "glass/src/glass.h"
#include "glass/src/math.h"
#include "widgets.h"
#include "stb_truetype.h"

typedef enum {
  GluiPrimitiveKindQuad = 0,
  GluiPrimitiveKindTexture,
} GluiPrimitiveKind;

typedef struct {
  GluiPrimitiveKind kind;
  Vec4              bounds;
  Vec4              color;
  Vec4              uv;
} GluiPrimitive;

typedef Da(GluiPrimitive) GluiPrimitives;

typedef struct {
  u32  _char;
  Vec2 size;
  Vec2 uv_size;
  f32  uv_x_pos;
  f32  text_size;
  f32  advance;
  f32  bearing_y;
} GluiGlyph;

typedef Da(GluiGlyph) GluiGlyphs;

typedef struct {
  Vec2            size;
  bool            redraw;
  GluiPrimitives  primitives;
  GluiPrimitives  prev_primitives;
  GluiGlyphs      glyphs;
  GlassShader     general_shader;
  GlassObject     general_object;
  GlassShader     texture_shader;
  GlassObject     texture_object;
  GlassTexture    glyphs_texture;
  u32             glyphs_texture_width;
  u32             glyphs_texture_height;
  u8             *glyphs_texture_buffer;
  stbtt_fontinfo  font;
  f32             font_scale;
} GluiRenderer;

GluiRenderer glui_init_renderer(Vec2 size, char *font_file_path);
void         glui_render(GluiRenderer *renderer, GluiWidget *root_widget);

#endif // GLUI_PRIMITIVES_H
