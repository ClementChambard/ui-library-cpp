#include "gp_circle.hpp"

void CircleGPWidget::render_at(glm::vec2 pos, CmdList &out_commands) const {
  if (m_color.a == 0)
    return;
  if (m_outline) {
    draw_rectangle_outline(out_commands, pos, m_current_size, m_color,
                           m_current_size.x / 2.f, m_outline_size);
  } else {
    draw_rectangle(out_commands, pos, m_current_size, m_color,
                   m_current_size.x / 2.f);
  }
}

void CircleGPWidget::lay(glm::vec2 min_size, glm::vec2 max_size) {
  f32 min_diameter = std::min(min_size.x, min_size.y);
  f32 max_diameter = std::min(max_size.x, max_size.y);
  assert(min_diameter <= max_diameter);
  f32 diameter = glm::clamp(m_radius * 2, min_diameter, max_diameter);
  m_current_size = {diameter, diameter};
}

GPWidget *CircleGPWidget::get_hovered_gp(glm::vec2 pos) {
  if (!m_has_mouse_event)
    return nullptr;
  pos -= (m_current_size / 2.f);
  f32 radius_sq = (m_current_size.x / 2.f) * (m_current_size.x / 2.f);
  f32 dist_sq = pos.x * pos.x + pos.y * pos.y;
  return dist_sq < radius_sq ? this : nullptr;
}

void CircleGPWidget::outline(f32 outline_size) {
  m_outline = true;
  m_outline_size = outline_size;
}
