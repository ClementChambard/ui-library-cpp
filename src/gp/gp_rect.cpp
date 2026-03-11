#include "gp_rect.hpp"

void RectGPWidget::render_at(glm::vec2 pos, CmdList &out_commands) const {
  if (m_color.a == 0) return;
  if (m_outline) {
    if (m_radius != 0.0) {
      draw_rectangle_outline(out_commands, pos, m_current_size, m_color,
                             m_radius, m_outline_size);
    } else {
      draw_rectangle_outline(out_commands, pos, m_current_size, m_color,
                             m_outline_size);
    }
  } else {
    if (m_radius != 0.0) {
      draw_rectangle(out_commands, pos, m_current_size, m_color, m_radius);
    } else {
      draw_rectangle(out_commands, pos, m_current_size, m_color);
    }
  }
}

void RectGPWidget::lay(glm::vec2 min_size, glm::vec2 max_size) {
  m_current_size = glm::clamp(m_size, min_size, max_size);
}

void RectGPWidget::outline(f32 outline_size) {
  m_outline = true;
  m_outline_size = outline_size;
}
