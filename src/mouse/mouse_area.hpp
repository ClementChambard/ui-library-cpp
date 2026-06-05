#pragma once

#include "../base/event.hpp"

namespace ui {

static constexpr u32 MB_LEFT = 1;

struct MouseEvent : Event {
  struct MouseArea *ma = nullptr;
  MouseEvent(event::Kind ev) : Event(ev) {}
};

struct MouseArea {
  MouseArea(Widget *parent) : parent(parent) {}
  Widget *parent;
  void dispatch_event(MouseEvent *e);
};

struct MouseButtonEvent : MouseEvent {
  u32 button_id = 0;
  glm::vec2 pos{};

  MouseButtonEvent(bool press, u32 button_id, glm::vec2 pos)
      : MouseEvent(press ? event::MOUSE_PRESS : event::MOUSE_RELEASE),
        button_id(button_id), pos(pos) {}
};

struct MouseDragEvent : MouseEvent {
  glm::vec2 pos;

  MouseDragEvent(glm::vec2 pos) : MouseEvent(event::MOUSE_DRAG), pos(pos) {}
};

} // namespace ui
