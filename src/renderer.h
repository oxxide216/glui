#ifndef GLUI_PRIMITIVES_H
#define GLUI_PRIMITIVES_H

#include <ft2build.h>
#include FT_FREETYPE_H

#include "glass/src/glass.h"
#include "glass/src/math.h"
#include "widgets.h"

typedef enum {
  GluiPrimitiveKindQuad = 0,
  GluiPrimitiveKindTexture,
} GluiPrimitiveKind;

typedef struct {
  GluiPrimitiveKind kind;
  Vec4              bounds;
  Vec4              color;
  u32               glyph_index;
} GluiPrimitive;

typedef Da(GluiPrimitive) GluiPrimitives;

typedef struct {
  u32           _char;
  u32           advance;
  UVec2         size;
  UVec2         bearing;
  GlassObject   object;
  GlassTextures textures;
  bool          used;
} GluiGlyph;

typedef Da(GluiGlyph) GluiGlyphs;

typedef struct {
  Vec2           size;
  bool           redraw;
  GluiPrimitives primitives;
  GluiPrimitives prev_primitives;
  GluiGlyphs     glyphs;
  GlassShader    general_shader;
  GlassObject    general_object;
  GlassShader    texture_shader;
  FT_Library     freetype;
  FT_Face        face;
} GluiRenderer;

GluiRenderer glui_init_renderer(Vec2 size, char *font_file_path);
void         glui_render(GluiRenderer *renderer, GluiWidget *root_widget);

#endif // GLUI_PRIMITIVES_H
