#include "winx/src/winx.h"
#include "winx/src/event.h"
#include "glui.h"
#include "widgets.h"
#include "shl_log.h"
#define SHL_STR_IMPLEMENTATION
#include "shl_str.h"

bool process_event(WinxEvent *event, Glui *glui) {
  if (event->kind == WinxEventKindQuit) {
    return false;
  } else if (event->kind == WinxEventKindResize) {
    u32 width = event->as.resize.width;
    u32 height = event->as.resize.height;

    glass_resize(width, height);
    glui_resize(glui, width, height);
  }

  return true;
}

int main(void) {
  Winx winx = winx_init();
  WinxWindow window = winx_init_window(&winx, STR_LIT("Glass test"),
                                       640, 480, WinxGraphicsModeOpenGL,
                                       NULL);
  glass_init();
  Glui glui = glui_init(window.width, window.height);

  GluiWidgetConfig config = { NULL, GluiAnchorTopLeft, {} };
  glui_button(&glui, STR_LIT("Click me!"), &config);

  bool is_running = true;
  while (is_running) {
    WinxEvent event;
    while ((event = winx_get_event(&window, false)).kind != WinxEventKindNone) {
      is_running = process_event(&event, &glui);
      if (!is_running)
        break;
    }

    glass_clear_screen(0.0, 0.0, 0.0, 1.0);
    glui_render(&glui);
    winx_draw(&window);
  }

  winx_destroy_window(&window);
  winx_cleanup(&winx);
  return 0;
}
