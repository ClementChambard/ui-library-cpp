#pragma once

#include "mouse_area.hpp"
#include <glm/glm.hpp>

namespace ui {

struct MARect : MouseArea {
  MARect(Widget *parent, glm::vec2 size) : MouseArea(parent), m_size(size) {}

  MouseArea *check(glm::vec2 pt, glm::vec2 *out_pos);

  void set_size(glm::vec2 s) { m_size = s; }
private:
  glm::vec2 m_size{};
};

} // namespace ui
