#include "winx/src/winx.h"
#include "winx/src/event.h"
#include "glui.h"
#include "shl_log.h"
#define SHL_STR_IMPLEMENTATION
#include "shl_str.h"

#define WHITE vec4(1.0, 1.0, 1.0, 1.0)
#define RED   vec4(1.0, 0.0, 0.0, 1.0)

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

void render_ui(Glui *glui) {
  glui_begin_block(glui, vec2(20.0, 20.0), GluiAnchorTop,
                   WHITE, vec2(640.0, 480.0));

  bool clicked = glui_button(glui, STR_LIT("Click me!"), RED, vec2(60.0, 30.0));
  if (clicked)
    INFO("Button was clicked!\n");

  glui_end_block(glui);
}

int main(void) {
  Winx winx = winx_init();
  WinxWindow window = winx_init_window(&winx, STR_LIT("Glass test"),
                                       640, 480, WinxGraphicsModeOpenGL,
                                       NULL);
  glass_init();
  Glui glui = glui_init(&window);

  bool is_running = true;
  while (is_running) {
    for (u32 i = 0; i < glui.events.len; ++i) {
      is_running = process_event(glui.events.items + i);
      if (!is_running)
        break;
    }

    glass_clear_screen(0.0, 0.0, 0.0, 1.0);
    render_ui(&glui);
    glui_next_frame(&glui);
    winx_draw(&window);
  }

  winx_destroy_window(&window);
  winx_cleanup(&winx);
  return 0;
}
