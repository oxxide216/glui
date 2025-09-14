#ifndef GLUI_MISC_H
#define GLUI_MISC_H

#include "glass/math.h"
#include "widgets.h"
#include "shl_defs.h"

bool glui_vec2_eq(Vec2 *a, Vec2 *b);
bool glui_vec3_eq(Vec3 *a, Vec3 *b);
bool glui_vec4_eq(Vec4 *a, Vec4 *b);

bool glui_style_eq(GluiStyle *a, GluiStyle *b);

#endif // GLUI_MISC_H
