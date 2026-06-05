#pragma once

#include "../defines.hpp"
#include "mouse_area.hpp"
#include <glm/glm.hpp>

namespace ui {

struct MouseEventDispatcher {
  MouseArea *cur = nullptr;
  MouseArea *dragging = nullptr;
  MouseArea *pressing = nullptr;

  glm::vec2 mouse_pos;

  void mouse_button_down(u32 button_id, glm::vec2 pos);
  void mouse_button_up(u32 button_id, glm::vec2 pos);
  void mouse_leave();
  void mouse_move(glm::vec2 pos, glm::vec2 rel);
};

} // namespace ui
