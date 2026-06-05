#include "shape_widget.hpp"

using namespace ui;

void ShapeWidget::render_at(glm::vec2 pos, CmdList &out_commands) const {
  if (m_color.a == 0)
    return;
  if (m_outline) {
    if (kind == CIRCLE) {
      draw_rectangle_outline(out_commands, pos, m_current_size, m_color,
                             m_current_size.x / 2.f, m_outline_size);
    } else if (kind == RECTANGLE) {
      if (rectangle.m_radius != 0.0) {
        draw_rectangle_outline(out_commands, pos, m_current_size, m_color,
                               rectangle.m_radius, m_outline_size);
      } else {
        draw_rectangle_outline(out_commands, pos, m_current_size, m_color,
                               m_outline_size);
      }
    }
  } else {
    if (kind == CIRCLE) {
      draw_rectangle(out_commands, pos, m_current_size, m_color,
                     m_current_size.x / 2.f);
    } else if (kind == RECTANGLE) {
      if (rectangle.m_radius != 0.0) {
        draw_rectangle(out_commands, pos, m_current_size, m_color,
                       rectangle.m_radius);
      } else {
        draw_rectangle(out_commands, pos, m_current_size, m_color);
      }
    }
  }
}

void ShapeWidget::lay(LayContext ctx) {
  if (kind == CIRCLE) {
    f32 min_diameter = std::min(ctx.min_size.x, ctx.min_size.y);
    f32 max_diameter = std::min(ctx.max_size.x, ctx.max_size.y);
    assert(min_diameter <= max_diameter);
    f32 diameter = glm::clamp(circle.m_radius * 2, min_diameter, max_diameter);
    m_current_size = {diameter, diameter};
  } else if (kind == RECTANGLE) {
    m_current_size = glm::clamp(rectangle.m_size, ctx.min_size, ctx.max_size);
  }
}

void ShapeWidget::outline(f32 outline_size) {
  m_outline = true;
  m_outline_size = outline_size;
}
