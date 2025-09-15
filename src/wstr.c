#include "wstr.h"

bool glui_wstr_eq(GluiWStr a, GluiWStr b) {
  if (a.len != b.len)
    return false;

  for (int i = 0; i < (int) a.len; ++i)
    if (a.ptr[i] != b.ptr[i])
      return false;

  return true;
}
