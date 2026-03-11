#include "gp.hpp"

GPWidget *GPWidget::get_hovered_gp(glm::vec2 pos) {
  if (pos.x < 0 || pos.y < 0)
    return nullptr;
  if (pos.x > m_current_size.x || pos.y > m_current_size.y)
    return nullptr;
  return m_has_mouse_event ? this : nullptr;
}
