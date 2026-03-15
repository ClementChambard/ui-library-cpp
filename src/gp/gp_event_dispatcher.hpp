#pragma once

#include "gp.hpp"

struct GPEventDispatcher {
  GPWidget *cur = nullptr;
  GPWidget *dragging = nullptr;
  GPWidget *pressing = nullptr;

  glm::vec2 mouse_pos;

  void mouse_button_down(u32 button_id, glm::vec2 pos);
  void mouse_button_up(u32 button_id, glm::vec2 pos);
  void mouse_leave();
  void mouse_move(glm::vec2 pos, glm::vec2 rel);
};
