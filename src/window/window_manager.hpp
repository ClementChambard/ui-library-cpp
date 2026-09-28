#pragma once

#include "../mouse/mouse_event_dispatcher.hpp"
#include "../render/main_window.hpp"
#include "window.hpp"

union SDL_Event;

namespace ui {

struct WindowManager {
  MouseArea *get_hovered_ma(glm::vec2 pos, glm::vec2 *out_pos = nullptr);
  void register_window(Window *w);
  void activate_window(Window *w);
  void render_all(CmdList &cmds);
  void handle_event(SDL_Event const &e);

  WindowManager();
  ~WindowManager();

  void focus(struct ControlWidget *w);
  void focus_next();
  void focus_prev();
  void unfocus();

  struct ControlWidget *m_current_focus = nullptr;

  render::MainWindow m_main_window;
  MouseEventDispatcher m_med;
  std::vector<Window *> m_windows;
  static WindowManager *INSTANCE;
};

} // namespace ui
