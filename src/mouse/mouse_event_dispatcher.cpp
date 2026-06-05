#include "mouse_event_dispatcher.hpp"
#include "../window/window_manager.hpp"

using namespace ui;

void MouseEventDispatcher::mouse_button_down(u32 button_id, glm::vec2 pos) {
  if (cur) {
    MouseButtonEvent mbe(true, button_id, pos);
    cur->dispatch_event(&mbe);
    pressing = cur;
  }
}

void MouseEventDispatcher::mouse_button_up(u32 button_id, glm::vec2 pos) {
  if (cur) {
    MouseButtonEvent mbe(false, button_id, pos);
    cur->dispatch_event(&mbe);
  }
  if (dragging) {
    MouseEvent e(event::MOUSE_DRAG_END);
    dragging->dispatch_event(&e);
  }
  pressing = nullptr;
  dragging = nullptr;
}

void MouseEventDispatcher::mouse_leave() {
  if (cur) {
    MouseEvent e(event::MOUSE_LEAVE);
    cur->dispatch_event(&e);
  }
  if (dragging) {
    MouseEvent e(event::MOUSE_DRAG_END);
    dragging->dispatch_event(&e);
  }
  cur = nullptr;
  dragging = nullptr;
  pressing = nullptr;
}

void MouseEventDispatcher::mouse_move(glm::vec2 pos, glm::vec2 rel) {
  mouse_pos = pos;
  if (pressing && !dragging) {
    dragging = pressing;
    MouseEvent e(event::MOUSE_DRAG_START);
    dragging->dispatch_event(&e);
  }
  if (dragging && rel != glm::vec2{0, 0}) {
    MouseDragEvent evt(rel);
    dragging->dispatch_event(&evt);
  }
  MouseArea *ma = WindowManager::INSTANCE->get_hovered_ma(pos);
  if (ma == cur)
    return;
  if (cur) {
    MouseEvent e(event::MOUSE_LEAVE);
    cur->dispatch_event(&e);
  }
  if (ma) {
    MouseEvent e(event::MOUSE_ENTER);
    ma->dispatch_event(&e);
  }
  cur = ma;
}
