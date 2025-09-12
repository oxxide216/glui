#ifndef GLUI_WSTR_H
#define GLUI_WSTR_H

#include "shl_defs.h"

#define GLUI_WSTR(ptr)                                                  \
  ((GluiWStr) { (WChar *) (ptr), sizeof(ptr) / sizeof(GluiWChar) - 1 })

typedef u32 GluiWChar;

typedef struct {
  GluiWChar *ptr;
  u32        len;
} GluiWStr;

#endif // GLUI_WSTR_H
