#pragma once

#include "../mouse/ma_rect.hpp"
#include "control_widget.hpp"

namespace ui {

struct Button : ControlWidget {
  Button(Widget *parent = nullptr);

  void render_at(glm::vec2 pos, CmdList &out_commands) const override;
  void lay(LayContext ctx) override;
  MouseArea *get_hovered_ma(glm::vec2 pos) override;
  void on_key(Key k) override;

  void set_text(std::string const &text);

  std::string m_text = "";
  Font *m_font = nullptr;

  enum Kind {
    PRIMARY,
    SECONDARY,
    // TODO: more
  } m_kind = SECONDARY;

private:
  bool m_hovering = false;
  bool m_pressing = false;
  // struct RectGPWidget *m_pick_rect = nullptr;
  MARect m_pick_rect{this, {}};

  void on_drag_end(Event *);
  void on_enter(Event *);
  void on_leave(Event *);
  void on_press(MouseButtonEvent *e);
  void on_release(MouseButtonEvent *e);
};

} // namespace ui
