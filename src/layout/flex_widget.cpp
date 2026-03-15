#include "flex_widget.hpp"

using namespace Flex;

void FlexWidget::render_at(glm::vec2 pos, CmdList &out_commands) const {
  for (auto const &c : m_children) {
    c.widget->render_at(pos + c.pos, out_commands);
  }
}

void FlexWidget::lay(LayContext ctx) {
  if (m_direction == HORIZONTAL) {
    lay_w_dir(ctx, &glm::vec2::x, &glm::vec2::y);
  } else {
    lay_w_dir(ctx, &glm::vec2::y, &glm::vec2::x);
  }
}

void FlexWidget::calc_min_max_size() {
  glm::vec2 size{};
  auto cross_axis = &glm::vec2::x;
  auto main_axis = &glm::vec2::y;
  if (m_direction == HORIZONTAL) {
    cross_axis = &glm::vec2::y;
    main_axis = &glm::vec2::x;
  }
  for (auto &c : m_children) {
    size.*main_axis += c.widget->m_minimum_size.*main_axis;
    size.*cross_axis =
        glm::max(c.widget->m_minimum_size.*cross_axis, size.*cross_axis);
  }

  size.*main_axis += m_gap * (m_children.size() - 1);
  size += glm::vec2(m_padding_left + m_padding_right,
                    m_padding_bottom + m_padding_top);

  set_min_max_size(size, {10000, 10000});
}

void FlexWidget::lay_w_dir(LayContext ctx, f32 glm::vec2::*main_axis,
                           f32 glm::vec2::*cross_axis) {
  m_current_size = glm::max(ctx.max_size, m_minimum_size);
  m_current_size = ctx.max_size;
  glm::vec2 gap_vec{};
  gap_vec.*main_axis = (m_children.size() - 1) * m_gap;
  auto rem_size = ctx.max_size -
                  glm::vec2{m_padding_left + m_padding_right,
                            m_padding_top + m_padding_bottom} -
                  gap_vec;
  auto cross_axis_max_size = rem_size.*cross_axis;
  auto rem_main = rem_size.*main_axis;
  struct FlexAssignment {
    Widget *w;
    f32 flex;
    f32 cur_flex_assigned = 0;
    bool done = false;
  };
  f32 total_flex_unit = 0.f;
  f32 flex_space = 0.f;
  std::vector<FlexAssignment> v;
  for (auto const &c : m_children) {
    rem_main -= c.widget->m_minimum_size.*main_axis;
    if (c.flex != 0) {
      v.push_back({c.widget, c.flex});
      total_flex_unit += c.flex;
      flex_space += c.widget->m_minimum_size.*main_axis;
    }
  }

  // assign remaining space
  glm::vec2 cursor = {m_padding_left, m_padding_top};
  f32 gap = m_gap;
  if (v.size() == 0) {
    if (m_main_axis_alignment == START) {
      // do nothing
    } else if (m_main_axis_alignment == CENTER) {
      cursor.*main_axis += rem_main / 2.f;
    } else if (m_main_axis_alignment == END) {
      cursor.*main_axis += rem_main;
    } else if (m_main_axis_alignment == SPACE_BETWEEN) {
      gap += rem_main / (m_children.size() - 1);
    } else if (m_main_axis_alignment == SPACE_AROUND) {
      f32 around_space = rem_main / m_children.size();
      gap += around_space;
      cursor.*main_axis += around_space / 2.f;
    } else {
      assert(false && "invalid main axis alignment value");
    }
  } else {
    f32 flex_unit_size = (flex_space + rem_main) / total_flex_unit;
    // TODO: better computations
    for (auto &f : v) {
      f.cur_flex_assigned =
          (flex_unit_size * f.flex) - f.w->m_minimum_size.*main_axis;
    }
  }

  u32 fa_cursor = 0;
  for (auto &c : m_children) {
    auto align = m_cross_axis_alignment;
    if (c.align_self != INHERIT)
      align = c.align_self;
    glm::vec2 size = c.widget->m_minimum_size;
    if (align == STRETCH)
      size.*cross_axis =
          glm::min(c.widget->m_maximum_size.*cross_axis, cross_axis_max_size);
    c.pos = cursor;
    if (align == START) {
      // do nothing
    } else if (align == END) {
      c.pos.*cross_axis += cross_axis_max_size - size.*cross_axis;
    } else {
      c.pos.*cross_axis += (cross_axis_max_size - size.*cross_axis) / 2.f;
    }
    if (c.flex) {
      auto &fa = v[fa_cursor++];
      size.*main_axis += fa.cur_flex_assigned;
    }
    c.widget->lay({ctx.current_window, size, size});
    cursor.*main_axis += size.*main_axis + gap;
  }
}
