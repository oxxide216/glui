#include "text-editor.h"

void glui_text_editor_move_left(GluiTextEditor *editor) {
  if (editor->cursor_pos > 0)
    --editor->cursor_pos;
}

void glui_text_editor_move_right(GluiTextEditor *editor) {
  if (editor->cursor_pos < editor->buffer.len)
    ++editor->cursor_pos;
}

void glui_text_editor_go_to(GluiTextEditor *editor, u32 pos) {
  editor->cursor_pos = pos;
  if (editor->cursor_pos > editor->buffer.len)
    editor->cursor_pos = editor->buffer.len;
}

void glui_text_editor_insert(GluiTextEditor *editor, u32 _char) {
  DA_INSERT(editor->buffer, _char, editor->cursor_pos);
  ++editor->cursor_pos;
}

void glui_text_editor_delete_prev(GluiTextEditor *editor) {
  if (editor->cursor_pos > 0) {
    DA_REMOVE_AT(editor->buffer, editor->cursor_pos - 1);
    --editor->cursor_pos;
  }
}

void glui_text_editor_delete_next(GluiTextEditor *editor) {
  if (editor->cursor_pos < editor->buffer.len)
    DA_REMOVE_AT(editor->buffer, editor->cursor_pos);
}
