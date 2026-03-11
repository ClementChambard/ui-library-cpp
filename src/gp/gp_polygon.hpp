#pragma once

#include "gp.hpp"

struct PolygonGPWidget : GPWidget {
  PolygonGPWidget(Widget *parent, std::vector<glm::vec2> points, Color color)
      : GPWidget(parent), m_points(points), m_color(color) {
    calculate_size();
    m_current_size = m_computed_size;
  }

  virtual void render_at(glm::vec2 pos, CmdList &out_commands) const override;
  virtual void lay(glm::vec2 min_size, glm::vec2 max_size) override;
  // better get_hovered_gp ??

  void calculate_size();

  std::vector<glm::vec2> m_points{};
  Color m_color{};
  glm::vec2 m_scale = {1.f, 1.f};
  glm::vec2 m_computed_size{};
};
