#pragma once

#include "../gp/gp_event_dispatcher.hpp"
#include "../render/main_window.hpp"
#include "window.hpp"

struct WindowManager {
  GPWidget *get_hovered_gp(glm::vec2 pos);
  void register_window(Window *w);
  void activate_window(Window *w);
  void render_all(CmdList &cmds);
  void handle_event(union SDL_Event const &e);

  WindowManager();
  ~WindowManager();

  void focus(struct ControlWidget *w);
  void focus_next();
  void focus_prev();
  void unfocus();

  struct ControlWidget *m_current_focus = nullptr;

  MainWindow m_main_window;
  GPEventDispatcher m_gped;
  std::vector<Window *> m_windows;
  static WindowManager *INSTANCE;
};
