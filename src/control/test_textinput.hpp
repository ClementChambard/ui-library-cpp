#pragma once

#include "control_widget.hpp"

struct TestTextInput : ControlWidget {
  TestTextInput(Widget *parent = nullptr);
  ~TestTextInput();

  void render_at(glm::vec2 pos, CmdList &out_commands) const override;
  void lay(LayContext ctx) override;
  struct GPWidget *get_hovered_gp(glm::vec2 pos) override;

  void on_key(Key k) override;
  void on_text(char const *text) override;

  void set_text(std::string const &text);

  std::string m_text = "";
  Font *m_font = nullptr;

private:
  struct RectGPWidget *m_pick_rect = nullptr;
};
