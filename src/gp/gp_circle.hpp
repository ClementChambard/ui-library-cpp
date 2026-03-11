#pragma once

#include "gp.hpp"

struct CircleGPWidget : GPWidget {
  CircleGPWidget(Widget *parent, f32 radius, Color color)
      : GPWidget(parent), m_radius(radius), m_color(color) {
    m_current_size = {2 * radius, 2 * radius};
  }

  virtual void render_at(glm::vec2 pos, CmdList &out_commands) const override;
  virtual void lay(glm::vec2 min_size, glm::vec2 max_size) override;
  GPWidget *get_hovered_gp(glm::vec2 pos) override;

  void outline(f32 outline_size = 1.f);

  bool m_outline = false;
  f32 m_outline_size = 1.f;
  f32 m_radius = 1.f;
  Color m_color{};
};
