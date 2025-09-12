#include "winx/src/winx.h"
#include "winx/src/event.h"
#include "glui.h"
#include "shl_log.h"
#define SHL_STR_IMPLEMENTATION
#include "shl_str.h"

#define MAIN_BG_COLOR    vec4(0.0, 0.0, 0.0, 1.0)
#define TEXT_COLOR       vec4(0.66, 0.7, 0.7, 1.0)
#define TEXT_SIZE        48.0

bool process_event(WinxEvent *event) {
  if (event->kind == WinxEventKindQuit) {
    return false;
  } else if (event->kind == WinxEventKindResize) {
    u32 width = event->as.resize.width;
    u32 height = event->as.resize.height;

    glass_resize(width, height);
  }

  return true;
}

void setup_styles(Glui *glui) {
  glui_get_style(glui, "text-editor")->bg_color = MAIN_BG_COLOR;
  glui_get_style(glui, "text-editor")->fg_color = TEXT_COLOR;
}

void render_ui(Glui *glui) {
  GluiTextEditor *editor = glui_text_editor(glui, TEXT_SIZE, "text-editor");
}

int main(void) {
  Winx winx = winx_init();
  WinxWindow window = winx_init_window(&winx, STR_LIT("Glass test"),
                                       640, 480, WinxGraphicsModeOpenGL,
                                       NULL);
  glass_init();
  Glui glui = glui_init(&window, "tests/FiraCode-Regular.ttf");
  setup_styles(&glui);

  bool is_running = true;
  while (is_running) {
    for (u32 i = 0; i < glui.events.len; ++i) {
      is_running = process_event(glui.events.items + i);
      if (!is_running)
        break;
    }

    render_ui(&glui);
    glui_next_frame(&glui);
    winx_draw(&window);
  }

  winx_destroy_window(&window);
  winx_cleanup(&winx);
  return 0;
}
