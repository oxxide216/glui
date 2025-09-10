#include "winx/src/winx.h"
#include "winx/src/event.h"
#include "glui.h"
#include "shl_log.h"
#define SHL_STR_IMPLEMENTATION
#include "shl_str.h"

#define MAIN_BG_COLOR    vec4(0.0, 0.0, 0.0, 1.0)
#define BUTTON_COLOR     vec4(0.16, 0.16, 0.16, 1.0)
#define BUTTON_ALT_COLOR vec4(0.1, 0.1, 0.1, 1.0)
#define TEXT_COLOR       vec4(0.66, 0.7, 0.7, 1.0)
#define TEXT_ALT_COLOR   vec4(0.26, 0.25, 0.3, 1.0)

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
  glui_get_style(glui, "root")->bg_color = MAIN_BG_COLOR;
  glui_get_style(glui, "block")->bg_color = MAIN_BG_COLOR;
  glui_get_style(glui, "button")->bg_color = BUTTON_COLOR;
  glui_get_style(glui, "button")->alt_bg_color = BUTTON_ALT_COLOR;
  glui_get_style(glui, "button")->fg_color = TEXT_COLOR;
  glui_get_style(glui, "button")->alt_fg_color = TEXT_ALT_COLOR;

  glui_get_style(glui, "text")->fg_color = TEXT_COLOR;
}

void render_ui(Glui *glui) {
  Vec4 bounds = vec4((glui->size.x - 160.0) / 2.0,
                     (glui->size.y - 320.0) / 2.0,
                     160.0,
                     320.0);

  glui_abs_bounds(glui, bounds);
  glui_begin_list(glui, GluiListKindVertical, vec2(20.0, 20.0), "block");

  glui_text(glui, STR_LIT("Some text"), "text");

  bool clicked0 = glui_button(glui, STR_LIT("Click me!"), "button");
  if (clicked0)
    INFO("Button 0 was clicked!\n");

  bool clicked1 = glui_button(glui, STR_LIT("Click me!"), "button");
  if (clicked1)
    INFO("Button 1 was clicked!\n");

  glui_button(glui, STR_LIT("Click me!"), "button");
  glui_button(glui, STR_LIT("Click me!"), "button");
  glui_button(glui, STR_LIT("Click me!"), "button");

  glui_end_list(glui);
}

int main(void) {
  Winx winx = winx_init();
  WinxWindow window = winx_init_window(&winx, STR_LIT("Glass test"),
                                       640, 480, WinxGraphicsModeOpenGL,
                                       NULL);
  glass_init();
  Glui glui = glui_init(&window, "tests/general/Hack-Regular.ttf");
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
