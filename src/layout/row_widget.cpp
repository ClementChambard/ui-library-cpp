#include "row_widget.hpp"

void RowWidget::render_at(glm::vec2 pos, CmdList &out_commands) const {
  for (auto const &c : m_children) {
    c.widget->render_at(pos + c.pos, out_commands);
  }
}

void RowWidget::calc_min_max_size() {
  glm::vec2 min_size{m_gap * (m_children.size() - 1), 0.f};
  for (auto &c : m_children) {
    min_size.y = glm::max(min_size.y, c.widget->m_minimum_size.y);
    min_size.x += c.widget->m_minimum_size.x;
  }
  set_min_max_size(min_size, {10000, 10000});
}

void RowWidget::lay(LayContext ctx) {
  f32 cur_x = 0;
  f32 max_y = 0;
  for (auto &c : m_children) {
    c.pos.y = 0;
    c.pos.x = cur_x;
    c.widget->lay({ctx.current_window,
                   {0, 0},
                   {glm::max(ctx.max_size.x - cur_x, 0.f), ctx.max_size.y}});
    cur_x += c.widget->m_current_size.x + m_gap;
    max_y = std::max(c.widget->m_current_size.y, max_y);
  }
  m_current_size = glm::clamp({cur_x, max_y}, ctx.min_size, ctx.max_size);
}
