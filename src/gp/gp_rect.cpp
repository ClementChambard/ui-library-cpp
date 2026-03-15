#include "gp_rect.hpp"

void RectGPWidget::render_at(glm::vec2 pos, CmdList &out_commands) const {
  if (m_color.a == 0)
    return;
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

void RectGPWidget::lay(LayContext ctx) {
  m_current_size = glm::clamp(m_size, ctx.min_size, ctx.max_size);
}

void RectGPWidget::outline(f32 outline_size) {
  m_outline = true;
  m_outline_size = outline_size;
}

RectGPWidget *RectGPWidget::make_event_handle(Widget *parent, glm::vec2 size) {
  static Widget *last_parent = nullptr;
  static u8 r, g;
  if (last_parent != parent) {
    last_parent = parent;
    r = 255;
    g = 0;
  } else {
    r -= 16;
    g += 16;
  }
  auto self = new RectGPWidget(nullptr, size, {r, g, 0, 128});
  self->m_parent = parent;
  return self;
}
