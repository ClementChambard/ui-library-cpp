#include "gp_polygon.hpp"

void PolygonGPWidget::render_at(glm::vec2 pos, CmdList &out_commands) const {
  if (m_color.a == 0)
    return;
  assert(m_points.size() >= 3);
  glm::vec2 c = pos + m_points[0] * m_scale;
  glm::vec2 last_point = pos + m_points[1] * m_scale;
  for (u32 i = 2; i < m_points.size(); i++) {
    glm::vec2 this_point = pos + m_points[i] * m_scale;
    draw_triangle(out_commands, c, last_point, this_point, m_color);
    last_point = this_point;
  }
}

void PolygonGPWidget::lay(glm::vec2 min_size, glm::vec2 max_size) {
  if (m_computed_size.x < max_size.x) {
    m_scale.x = max_size.x / m_computed_size.x;
  }
  if (m_computed_size.y < max_size.y) {
    m_scale.y = max_size.y / m_computed_size.y;
  }
  m_current_size = glm::clamp(min_size, m_computed_size * m_scale, max_size);
}

void PolygonGPWidget::calculate_size() {
  m_computed_size = {0, 0};
  for (auto const &p : m_points) {
    if (p.x > m_computed_size.x)
      m_computed_size.x = p.x;
    if (p.y > m_computed_size.y)
      m_computed_size.y = p.y;
  }
}
