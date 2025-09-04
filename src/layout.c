#include "layout.h"

GluiLayout glui_init_layout(Vec2 size) {
  GluiLayout layout = {0};
  layout.size = size;
  return layout;
}

Vec4 glui_compute_bounds(GluiLayout *layout, Vec2 size) {
  if (layout->blocks.len == 0)
    return vec4(0.0, 0.0, layout->size.x, layout->size.y);

  GluiBlock *parent = layout->blocks.items + layout->blocks.len - 1;
  Vec2 prev_free_space_pos = parent->free_space_pos;
  Vec2 prev_free_space_size = parent->free_space_size;
  Vec2 pos = {0};
  bool limited_space_x = parent->free_space_size.x < size.x + parent->margin.x * 2.0;
  bool limited_space_y = parent->free_space_size.y < size.y + parent->margin.y * 2.0;

  if (parent->fill_x || limited_space_x) {
    size.x = prev_free_space_size.x - parent->margin.x * 2.0;
    if (size.x < 0.0)
      size.x = 0.0;
  }

  if (parent->fill_y || limited_space_y) {
    size.y = prev_free_space_size.y - parent->margin.y * 2.0;
    if (size.y < 0.0)
      size.y = 0.0;
  }

  switch (parent->anchor_x) {
  case GluiAnchorXLeft: {
    pos.x = prev_free_space_pos.x + parent->margin.x;

    parent->free_space_pos.x += size.x + parent->margin.x;
    parent->free_space_size.x -= size.x + parent->margin.x;
  } break;

  case GluiAnchorXRight: {
    pos.x = prev_free_space_pos.x + prev_free_space_size.x -
            size.x - parent->margin.x;

    parent->free_space_size.x -= size.x + parent->margin.x;
  } break;
  }

  switch (parent->anchor_y) {
  case GluiAnchorYTop: {
    pos.y = prev_free_space_pos.y + parent->margin.y;

    parent->free_space_pos.y += size.y + parent->margin.y;
    parent->free_space_size.y -= size.y + parent->margin.y;
  } break;

  case GluiAnchorYBottom: {
    pos.y = prev_free_space_pos.y + prev_free_space_size.y -
            size.y - parent->margin.y;

    parent->free_space_size.y -= size.y + parent->margin.y;
  } break;
  }

  return vec4(pos.x, pos.y, size.x, size.y);
}

void glui_push_block(GluiLayout *layout, Vec4 bounds, Vec2 margin,
                     GluiAnchorX anchor_x, GluiAnchorY anchor_y,
                     bool fill_x, bool fill_y) {
  GluiBlock block = {
    bounds,
    margin,
    vec2(bounds.x, bounds.y),
    vec2(bounds.z, bounds.w),
    anchor_x,
    anchor_y,
    fill_x,
    fill_y,
  };
  DA_APPEND(layout->blocks, block);
}

void glui_pop_block(GluiLayout *layout) {
  if (layout->blocks.len > 0)
    --layout->blocks.len;
}
