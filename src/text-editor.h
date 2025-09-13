#ifndef GLUI_TEXT_EDITOR_H
#define GLUI_TEXT_EDITOR_H

#include "wstr.h"
#include "shl_defs.h"

typedef Da(GluiWChar) GluiTextEditorLine;

typedef Da(GluiTextEditorLine) GluiTextEditorLines;

typedef struct {
  GluiTextEditorLines lines;
  u32                 row, col;
} GluiTextEditor;

void glui_text_editor_move_left(GluiTextEditor *editor);
void glui_text_editor_move_right(GluiTextEditor *editor);
void glui_text_editor_move_down(GluiTextEditor *editor);
void glui_text_editor_move_up(GluiTextEditor *editor);
void glui_text_editor_insert(GluiTextEditor *editor, GluiWChar _char);
void glui_text_editor_delete_prev(GluiTextEditor *editor);
void glui_text_editor_delete_next(GluiTextEditor *editor);

#endif // GLUI_TEXT_EDITOR_H
