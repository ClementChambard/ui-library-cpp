#pragma once

#include "gp.hpp"

struct GPEventDispatcher {
  GPWidget *cur = nullptr;
  GPWidget *dragging = nullptr;
  GPWidget *pressing = nullptr;

  void mouse_button_down(u32 button_id, glm::vec2 pos);
  void mouse_button_up(u32 button_id, glm::vec2 pos);
  void mouse_leave();
  void mouse_move(Widget *root, glm::vec2 pos, glm::vec2 rel);
};
