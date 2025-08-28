#include "vertices.h"

u32 glui_gen_quad_vertices(Vec2 **vertices, Vec2 pos, Vec2 size) {
  (*vertices)[0] = vec2(pos.x, pos.y);
  (*vertices)[1] = vec2(pos.x + size.x, pos.y);
  (*vertices)[2] = vec2(pos.x, pos.y + size.y);
  (*vertices)[3] = vec2(pos.x + size.x, pos.y + size.y);

  return 4 * sizeof(Vec2);
}
