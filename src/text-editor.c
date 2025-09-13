#include "text-editor.h"

void glui_text_editor_move_left(GluiTextEditor *editor) {
  if (editor->col > 0) {
    --editor->col;
  } else if (editor->row > 0) {
    editor->col = editor->lines.items[--editor->row].len;
  }
}

void glui_text_editor_move_right(GluiTextEditor *editor) {
  if (editor->col < editor->lines.items[editor->row].len) {
    ++editor->col;
  } else if (editor->row + 1 < editor->lines.len) {
    ++editor->row;
    editor->col = 0;
  }
}

void glui_text_editor_move_down(GluiTextEditor *editor) {
  if (editor->row + 1 < editor->lines.len) {
    ++editor->row;
    if (editor->col > editor->lines.items[editor->row].len)
      editor->col = editor->lines.items[editor->row].len;
  }
}

void glui_text_editor_move_up(GluiTextEditor *editor) {
  if (editor->row > 0) {
    --editor->row;
    if (editor->col > editor->lines.items[editor->row].len)
      editor->col = editor->lines.items[editor->row].len;
  }
}

void glui_text_editor_insert(GluiTextEditor *editor, GluiWChar _char) {
  if (_char == '\n') {
    DA_INSERT(editor->lines, (GluiTextEditorLine) {0}, editor->row + 1);
    GluiTextEditorLine *line = editor->lines.items + editor->row;
    GluiTextEditorLine *next_line = editor->lines.items + editor->row + 1;
    next_line->len = line->len - editor->col;
    next_line->cap = next_line->len;
    next_line->items = malloc(next_line->cap * sizeof(GluiWChar));
    memcpy(next_line->items, line->items + editor->col,
           next_line->len * sizeof(GluiWChar));
    line->len = editor->col;
    ++editor->row;
    editor->col = 0;
  } else {
    DA_INSERT(editor->lines.items[editor->row], _char, editor->col);
    ++editor->col;
  }
}

void glui_text_editor_delete_prev(GluiTextEditor *editor) {
  if (editor->col > 0) {
    DA_REMOVE_AT(editor->lines.items[editor->row], editor->col - 1);
    --editor->col;
  } else if (editor->row > 0) {
    GluiTextEditorLine *prev_line = editor->lines.items + editor->row - 1;
    GluiTextEditorLine *line = editor->lines.items + editor->row;
    if (prev_line->cap < prev_line->len + line->len) {
      prev_line->cap = prev_line->len + line->len;
      prev_line->items = realloc(prev_line->items, prev_line->cap);
    }
    memcpy(prev_line->items + prev_line->len, line->items,
           line->len * sizeof(u32));
    editor->col = prev_line->len;
    prev_line->len += line->len;
    free(line->items);
    DA_REMOVE_AT(editor->lines, editor->row);
    --editor->row;
  }
}

void glui_text_editor_delete_next(GluiTextEditor *editor) {
  if (editor->col < editor->lines.items[editor->row].len) {
    DA_REMOVE_AT(editor->lines.items[editor->row], editor->col);
  } else if (editor->col + 1 < editor->lines.len) {
    GluiTextEditorLine *line = editor->lines.items + editor->row;
    GluiTextEditorLine *next_line = editor->lines.items + editor->row + 1;
    if (line->cap < line->len + next_line->len) {
      line->cap = line->len + next_line->len;
      line->items = realloc(line->items, line->cap);
    }
    memcpy(line->items + line->len, next_line->items,
           next_line->len * sizeof(u32));
    line->len += next_line->len;
    free(next_line->items);
    DA_REMOVE_AT(editor->lines, editor->row + 1);
  }
}
