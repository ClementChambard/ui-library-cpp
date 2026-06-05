#pragma once

#include "window.hpp"

namespace ui {

struct Panel : Window {
  Panel();

  void render_at(glm::vec2 pos, CmdList &out_commands) const override;
  void recalc_layout(glm::vec2 size) override;
  MouseArea *get_hovered_ma(glm::vec2 pos) override;
  void calc_min_max_size() override;

  f32 m_saved_height = 0.f;
  u32 m_current_panel_button = 0;
  bool m_collapsed = false;
  std::string m_title;

  MARect m_panel_buttons[1] = {{this, {}}};
};

} // namespace ui
