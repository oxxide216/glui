#ifndef GLUI_LAYOUT_H
#define GLUI_LAYOUT_H

#include "glass/src/math.h"

typedef enum {
  GluiAnchorLeft = 0,
  GluiAnchorRight,
  GluiAnchorTop,
  GluiAnchorBottom,
} GluiAnchor;

typedef struct {
  Vec4       bounds;
  Vec2       margin;
  Vec2       free_space_pos;
  Vec2       free_space_size;
  GluiAnchor anchor;
} GluiBlock;

typedef Da(GluiBlock) GluiBlocks;

typedef struct {
  GluiBlocks blocks;
  Vec2       size;
  Vec4       margin;
} GluiLayout;

GluiLayout glui_init_layout(Vec2 size);
void       glui_reset_layout(GluiLayout *layout);

Vec4 glui_compute_bounds(GluiLayout *layout, Vec2 size, bool fill);
void glui_push_block(GluiLayout *layout, Vec4 bounds, Vec2 margin, GluiAnchor anchor);
void glui_pop_block(GluiLayout *layout);

#endif // GLUI_LAYOUT_H
