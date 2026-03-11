#pragma once

#include "../base/widget.hpp"

struct GPWidget : Widget {
  bool m_has_mouse_event = true;
  GPWidget(Widget *parent) : Widget(parent) {}
  GPWidget *get_hovered_gp(glm::vec2 pos) override;
};
