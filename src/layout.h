#ifndef GLUI_LAYOUT_H
#define GLUI_LAYOUT_H

#include "glass/src/math.h"

typedef enum {
  GluiAnchorXLeft = 0,
  GluiAnchorXRight,
} GluiAnchorX;

typedef enum {
  GluiAnchorYTop = 0,
  GluiAnchorYBottom,
} GluiAnchorY;

typedef struct {
  Vec4        bounds;
  Vec2        margin;
  Vec2        free_space_pos;
  Vec2        free_space_size;
  GluiAnchorX anchor_x;
  GluiAnchorY anchor_y;
  bool        fill_x;
  bool        fill_y;
} GluiBlock;

typedef Da(GluiBlock) GluiBlocks;

typedef struct {
  GluiBlocks blocks;
  Vec2       size;
  Vec4       margin;
} GluiLayout;

GluiLayout glui_init_layout(Vec2 size);

Vec4 glui_compute_bounds(GluiLayout *layout, Vec2 size);
void glui_push_block(GluiLayout *layout, Vec4 bounds, Vec2 margin,
                     GluiAnchorX anchor_x, GluiAnchorY anchor_y,
                     bool fill_x, bool fill_y);
void glui_pop_block(GluiLayout *layout);

#endif // GLUI_LAYOUT_H
