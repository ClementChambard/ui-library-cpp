#pragma once

#include "../mouse/ma_rect.hpp"
#include "control_widget.hpp"

namespace ui {

struct TestTextInput : ControlWidget {
  TestTextInput(Widget *parent = nullptr);

  void render_at(glm::vec2 pos, CmdList &out_commands) const override;
  void lay(LayContext ctx) override;
  MouseArea *get_hovered_ma(glm::vec2 pos) override;

  void on_key(Key k) override;
  void on_text(char const *text) override;

  void set_text(std::string const &text);

  std::string m_text = "";
  Font *m_font = nullptr;

private:
  MARect m_pick_rect{this, {}};
};

} // namespace ui
