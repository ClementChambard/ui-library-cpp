#include "window.hpp"

void Window::render_at(glm::vec2 pos, CmdList &out_commands) const {
  pos += m_pos;
  draw_set_scissor(out_commands, pos, m_current_size);

  m_content->render_at(pos, out_commands);

  draw_disable_scissor(out_commands);
}

GPWidget *Window::get_hovered_gp(glm::vec2 pos) {
  return m_content->get_hovered_gp(pos - m_pos);
}

void Window::recalc_layout() {
  if (m_content)
    m_content->lay(m_current_size, m_current_size);
}

void Window::set_size(glm::vec2 size) {
  if (size.x < m_min_size.x)
    size.x = m_min_size.x;
  if (size.y < m_min_size.y)
    size.y = m_min_size.y;
  m_current_size = size;
  recalc_layout();
}
