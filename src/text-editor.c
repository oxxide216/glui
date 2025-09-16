#include <ctype.h>

#include "text-editor.h"

static GluiTextEditorLine *glui_get_line(GluiTextEditor *editor, u32 row) {
  return editor->lines.items + row;
}

void glui_text_editor_move_left(GluiTextEditor *editor) {
  if (editor->col > 0) {
    --editor->col;
  } else if (editor->row > 0) {
    editor->col = glui_get_line(editor, --editor->row)->len;
  }
}

void glui_text_editor_move_right(GluiTextEditor *editor) {
  if (editor->col < glui_get_line(editor, editor->row)->len) {
    ++editor->col;
  } else if (editor->row + 1 < editor->lines.len) {
    ++editor->row;
    editor->col = 0;
  }
}

void glui_text_editor_move_down(GluiTextEditor *editor) {
  if (editor->row + 1 < editor->lines.len) {
    ++editor->row;
    if (editor->col > glui_get_line(editor, editor->row)->len)
      editor->col = glui_get_line(editor, editor->row)->len;
  }
}

void glui_text_editor_move_up(GluiTextEditor *editor) {
  if (editor->row > 0) {
    if (editor->col > glui_get_line(editor, --editor->row)->len)
      editor->col = glui_get_line(editor, editor->row)->len;
  }
}

void glui_text_editor_move_left_word(GluiTextEditor *editor, bool delete) {
  if (editor->col == 0 && editor->row > 0) {
    editor->col = glui_get_line(editor, --editor->row)->len;
  }

  bool find_alnum = false;
  while (editor->col > 0) {
    if (isalnum(glui_get_line(editor, editor->row)->items[editor->col - 1]))
      find_alnum = true;
    else if (find_alnum)
      break;

    if (delete) {
      GluiTextEditorLine *line = glui_get_line(editor, editor->row);
      DA_REMOVE_AT(*line, editor->col);
    }

    --editor->col;
  }
}

void glui_text_editor_move_right_word(GluiTextEditor *editor, bool delete) {
  if (editor->col == glui_get_line(editor, editor->row)->len &&
      editor->row + 1 < editor->lines.len) {
    ++editor->row;
    editor->col = 0;
  }

  bool find_alnum = false;
  while (editor->col < glui_get_line(editor, editor->row)->len) {
    if (isalnum(glui_get_line(editor, editor->row)->items[editor->col]))
      find_alnum = true;
    else if (find_alnum)
      break;

    if (delete) {
      GluiTextEditorLine *line = glui_get_line(editor, editor->row);
      DA_REMOVE_AT(*line, editor->col);
    }

    if (!delete)
      ++editor->col;
  }
}

void glui_text_editor_move_down_paragraph(GluiTextEditor *editor) {
  bool find_paragraph = false;
  while (editor->row + 1 < editor->lines.len) {
    if (glui_get_line(editor, editor->row)->len > 0)
      find_paragraph = true;
    else if (find_paragraph)
      break;

    ++editor->row;
  }

  GluiTextEditorLine *line = glui_get_line(editor, editor->row);
  if (editor->col > line->len)
    editor->col = line->len;
}

void glui_text_editor_move_up_paragraph(GluiTextEditor *editor) {
  bool find_paragraph = false;
  while (editor->row > 0) {
    if (glui_get_line(editor, editor->row - 1)->len > 0)
      find_paragraph = true;
    else if (find_paragraph)
      break;

    --editor->row;
  }

  GluiTextEditorLine *line = glui_get_line(editor, editor->row);
  if (editor->col > line->len)
    editor->col = line->len;
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
