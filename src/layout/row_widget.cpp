#include "row_widget.hpp"

void RowWidget::render_at(glm::vec2 pos, CmdList &out_commands) const {
  for (auto const &c : m_children) {
    c.widget->render_at(pos + c.pos, out_commands);
  }
}

void RowWidget::lay(glm::vec2 min_size, glm::vec2 max_size) {
  f32 cur_x = 0;
  f32 max_y = 0;
  for (auto &c : m_children) {
    c.pos.y = 0;
    c.pos.x = cur_x;
    c.widget->lay({0, 0}, {max_size.x - cur_x, max_size.y});
    cur_x += c.widget->m_current_size.x;
    max_y = std::max(c.widget->m_current_size.y, max_y);
  }
  m_current_size = glm::clamp({cur_x, max_y}, min_size, max_size);
}
