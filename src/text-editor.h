#ifndef GLUI_TEXT_EDITOR_H
#define GLUI_TEXT_EDITOR_H

#include "shl_defs.h"

typedef Da(u32) GluiTextEditorBuffer;

typedef struct {
  GluiTextEditorBuffer buffer;
  u32                  cursor_pos;
} GluiTextEditor;

void glui_text_editor_move_left(GluiTextEditor *editor);
void glui_text_editor_move_right(GluiTextEditor *editor);
void glui_text_editor_go_to(GluiTextEditor *editor, u32 pos);
void glui_text_editor_insert(GluiTextEditor *editor, u32 _char);
void glui_text_editor_delete_prev(GluiTextEditor *editor);
void glui_text_editor_delete_next(GluiTextEditor *editor);

#endif // GLUI_TEXT_EDITOR_H
