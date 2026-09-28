#pragma once

#include "window.hpp"

namespace ui {

struct Dialog : Window {
  Dialog();
  ~Dialog();

  void render_at(glm::vec2 pos, CmdList &out_commands) const override;
  void recalc_layout(glm::vec2 size) override;
  MouseArea *get_hovered_ma(glm::vec2 pos, glm::vec2 *out_pos = nullptr) override;
  void calc_min_max_size() override;

  void set_standard_buttons(u32 v);
  void set_modal(bool v);

  void on_standard_button_click(u32 std_id);

  bool m_is_modal = false;

  struct StandardButtons *m_standard_buttons = nullptr;

  std::string m_title;
};

} // namespace ui
