#include "layout.h"

GluiLayout glui_init_layout(Vec2 size) {
  GluiLayout layout = {0};

  layout.size = size;
  glui_reset_layout(&layout);

  return layout;
}

void glui_reset_layout(GluiLayout *layout) {
  if (layout->blocks.items)
    free(layout->blocks.items);
  layout->blocks = (GluiBlocks) {0};

  GluiBlock block = {
    vec4(0.0, 0.0, layout->size.x, layout->size.y),
    vec2(0.0, 0.0),
    vec2(0.0, 0.0),
    vec2(layout->size.x, layout->size.y),
    GluiAnchorTop,
  };
  DA_APPEND(layout->blocks, block);
}

Vec4 glui_compute_bounds(GluiLayout *layout, Vec2 size, bool fill) {
  GluiBlock *parent = layout->blocks.items + layout->blocks.len - 1;
  Vec2 pos = {0};

  switch (parent->anchor) {
  case GluiAnchorTop: {
    pos.x = parent->free_space_pos.x + parent->margin.x +
            (parent->free_space_size.x - size.x) / 2;
    pos.y = parent->free_space_pos.y + parent->margin.y;
  } break;
  }

  return vec4(pos.x, pos.y, size.x, size.y);
}

void glui_push_block(GluiLayout *layout, Vec4 bounds, Vec2 margin, GluiAnchor anchor) {
  GluiBlock block = {
    bounds,
    margin,
    vec2(bounds.x, bounds.y),
    vec2(bounds.z, bounds.w),
    anchor,
  };
  DA_APPEND(layout->blocks, block);
}

void glui_pop_block(GluiLayout *layout) {
  if (layout->blocks.len > 1)
    --layout->blocks.len;
}
