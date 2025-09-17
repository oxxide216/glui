#include "winx/winx.h"
#include "winx/event.h"
#include "glui/glui.h"
#include "io.h"
#include "shl_log.h"
#define SHL_STR_IMPLEMENTATION
#include "shl_str.h"

#define ROOT_BG_COLOR vec4(0.0, 0.0, 0.0, 0.0)
#define MAIN_BG_COLOR vec4(0.1, 0.1, 0.1, 0.8)
#define TEXT_COLOR    vec4(0.5, 0.5, 0.7, 1.0)
#define TEXT_SIZE     36.0
#define SCROLL_SPEED  15.0

typedef enum {
  ActionNone = 0,
  ActionQuit,
  ActionSave,
  ActionOpen,
} Action;

Action process_event(WinxEvent *event, bool *is_ctrl_pressed, f32 *scroll) {
  if (event->kind == WinxEventKindQuit) {
    return ActionQuit;
  } else if (event->kind == WinxEventKindResize) {
    u32 width = event->as.resize.width;
    u32 height = event->as.resize.height;

    glass_resize(width, height);
  } else if (event->kind == WinxEventKindKeyPress) {
    if (event->as.key.key_code == WinxKeyCodeLeftControl)
      *is_ctrl_pressed = true;
    else if (event->as.key.key_code == WinxKeyCodeS && *is_ctrl_pressed)
      return ActionSave;
    else if (event->as.key.key_code == WinxKeyCodeO && *is_ctrl_pressed)
      return ActionOpen;
  } else if (event->kind == WinxEventKindKeyRelease) {
    if (event->as.key.key_code == WinxKeyCodeLeftControl)
      *is_ctrl_pressed = false;
  } else if (event->kind == WinxEventKindButtonPress) {
    if (event->as.button.button == WinxMouseButtonWheelUp)
      *scroll -= SCROLL_SPEED;
    else if (event->as.button.button == WinxMouseButtonWheelDown)
      *scroll += SCROLL_SPEED;

    if (*scroll < 0.0)
      *scroll = 1.0;
  }

  return ActionNone;
}

void setup_styles(Glui *glui) {
  glui_get_style(glui, "root")->bg_color = ROOT_BG_COLOR;
  glui_get_style(glui, "block")->bg_color = MAIN_BG_COLOR;
  glui_get_style(glui, "text-editor")->bg_color = ROOT_BG_COLOR;
  glui_get_style(glui, "text-editor")->fg_color = TEXT_COLOR;
}

void render_ui(Glui *glui, GluiTextEditor **editor, f32 scroll) {
  glui_begin_list(glui, GluiListKindHorizontal, vec2(5.0, 5.0), "block");

  *editor = glui_text_editor(glui, TEXT_SIZE, vec2(0.0, scroll), "text-editor");

  glui_end_list(glui);
}

int main(void) {
  Winx winx = winx_init();
  WinxWindow window = winx_init_window(&winx, STR_LIT("Glass test"),
                                       640, 480, WinxGraphicsModeOpenGL,
                                       NULL);
  glass_init();
  Glui glui = glui_init(&window, "tests/JetBrainsMono-Regular.ttf");
  setup_styles(&glui);

  GluiTextEditor *editor = NULL;;
  bool is_ctrl_pressed = false;
  f32 scroll = 0.0;
  bool is_running = true;

  while (is_running) {
    for (u32 i = 0; i < glui.events.len; ++i) {
      Action action = process_event(glui.events.items + i,
                                    &is_ctrl_pressed,
                                    &scroll);

      if (action == ActionQuit) {
        is_running = false;
        break;
      } else if (action == ActionSave && editor) {
        StringBuilder sb = {0};
        for (u32 i = 0; i < editor->lines.len; ++i) {
          if (i > 0)
            sb_push_char(&sb, '\n');

          Str line_str = {
            malloc(editor->lines.items[i].len),
            editor->lines.items[i].len,
          };

          for (u32 j = 0; j < editor->lines.items[i].len; ++j) {
            line_str.ptr[j] = editor->lines.items[i].items[j];
          }

          sb_push_str(&sb, line_str);
        }

        write_file("text.txt", sb_to_str(sb));
      } else if (action == ActionOpen && editor) {
        editor->lines.len = 1;
        GluiTextEditorLine *line = editor->lines.items;
        line->len = 0;

        Str content = read_file("text.txt");
        for (u32 j = 0; j < content.len; ++j) {
          if (content.ptr[j] == '\n') {
            DA_APPEND(editor->lines, (GluiTextEditorLine) {0});
            ++line;
            continue;
          }

          DA_APPEND(*line, (GluiWChar) content.ptr[j]);
        }
      }
    }

    render_ui(&glui, &editor, scroll);
    glui_next_frame(&glui);
    winx_draw(&window);
  }

  winx_destroy_window(&window);
  winx_cleanup(&winx);
  return 0;
}
