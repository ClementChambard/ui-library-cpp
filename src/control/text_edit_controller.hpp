#pragma once

#include "control_widget.hpp"
#include <string>

namespace ui {

struct TextEditControllerSettings {
  bool multiline = false;
  bool send_change = true;
  bool can_select = true;

  static const TextEditControllerSettings Default;
};

struct TextEditController {

  bool on_key(Key k, Widget *sender);
  bool on_text(char const *text, Widget *sender);

  void set_text(std::string const &text) { m_text = text; }

  void render_text(glm::vec2 pos, CmdList &out_commands, Font *fnt,
                   Color col) const;

  bool mouse_event(Event *e);
  void clear_selection() { m_selecting = false; }
  void selection_start() {
    m_selecting = true;
    m_mouse_end_sel_pos = m_last_mouse_pos;
  }
  void update_selection(glm::vec2 mouse_delta);

  std::string m_text;
  glm::vec2 m_rect;
  glm::vec2 m_last_mouse_pos;
  glm::vec2 m_mouse_end_sel_pos;
  bool m_selecting = false;
  bool m_active = false;
  TextEditControllerSettings const *m_settings =
      &TextEditControllerSettings::Default;
};

} // namespace ui
