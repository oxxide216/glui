#include "misc.h"

bool glui_vec2_eq(Vec2 *a, Vec2 *b) {
  return a->x == b->x && a->y == b->y;
}

bool glui_vec3_eq(Vec3 *a, Vec3 *b) {
  return a->x == b->x && a->y == b->y &&
         a->z == b->z;
}

bool glui_vec4_eq(Vec4 *a, Vec4 *b) {
  return a->x == b->x && a->y == b->y &&
         a->z == b->z && a->w == b->w;
}

bool glui_style_eq(GluiStyle *a, GluiStyle *b) {
  return strcmp(a->class, b->class) == 0 &&
         glui_vec4_eq(&a->bg_color, &b->bg_color) &&
         glui_vec4_eq(&a->alt_bg_color, &b->alt_bg_color) &&
         glui_vec4_eq(&a->fg_color, &b->fg_color) &&
         glui_vec4_eq(&a->alt_fg_color, &b->alt_fg_color);
}
