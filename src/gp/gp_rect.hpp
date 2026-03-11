#pragma once

#include "gp.hpp"

struct RectGPWidget : GPWidget {
  RectGPWidget(Widget *parent, glm::vec2 size, Color color)
      : GPWidget(parent), m_size(size), m_color(color) {
    m_current_size = size;
  }

  virtual void render_at(glm::vec2 pos, CmdList &out_commands) const override;
  virtual void lay(glm::vec2 min_size, glm::vec2 max_size) override;

  void outline(f32 outline_size = 1.f);

  bool m_outline = false;
  f32 m_outline_size = 1.f;
  f32 m_radius = 0.f; // TODO: different radius per corner ??
  glm::vec2 m_size{};
  Color m_color{};
};
