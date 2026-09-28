#pragma once

#include "../mouse/ma_rect.hpp"
#include "control_widget.hpp"
#include "text_edit_controller.hpp"

namespace ui {

struct TextInput : ControlWidget {
  TextInput(Widget *parent = nullptr);

  void render_at(glm::vec2 pos, CmdList &out_commands) const override;
  void lay(LayContext ctx) override;
  MouseArea *get_hovered_ma(glm::vec2 pos,
                            glm::vec2 *out_pos = nullptr) override {

    return m_pick_rect.check(pos, out_pos);
  }
  void focus() override {
    ControlWidget::focus();
    m_controller.m_active = true;
  }
  void unfocus() override {
    ControlWidget::unfocus();
    m_controller.m_active = false;
    m_controller.clear_selection();
  }

  void on_key(Key k) override;
  void on_text(char const *text) override;
  void set_text(std::string const &text);

  TextEditController m_controller;
  Font *m_font = nullptr;

private:
  MARect m_pick_rect{this, {}};
};

} // namespace ui
