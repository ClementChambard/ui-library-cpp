#include "gp_event_dispatcher.hpp"
#include "../window/window_manager.hpp"

void GPEventDispatcher::mouse_button_down(u32 button_id, glm::vec2 pos) {
  if (cur) {
    MouseButtonEvent mbe(true, button_id, pos);
    cur->dispatch_event(&mbe);
    pressing = cur;
  }
}

void GPEventDispatcher::mouse_button_up(u32 button_id, glm::vec2 pos) {
  if (cur) {
    MouseButtonEvent mbe(false, button_id, pos);
    cur->dispatch_event(&mbe);
  }
  if (dragging) {
    Event e(MOUSE_DRAG_END_EVENT);
    dragging->dispatch_event(&e);
  }
  pressing = nullptr;
  dragging = nullptr;
}

void GPEventDispatcher::mouse_leave() {
  if (cur) {
    Event e(MOUSE_LEAVE_EVENT);
    cur->dispatch_event(&e);
  }
  if (dragging) {
    Event e(MOUSE_DRAG_END_EVENT);
    dragging->dispatch_event(&e);
  }
  cur = nullptr;
  dragging = nullptr;
  pressing = nullptr;
}

void GPEventDispatcher::mouse_move(glm::vec2 pos, glm::vec2 rel) {
  mouse_pos = pos;
  if (pressing && !dragging) {
    dragging = pressing;
    Event e(MOUSE_DRAG_START_EVENT);
    dragging->dispatch_event(&e);
  }
  if (dragging && rel != glm::vec2{0, 0}) {
    MouseDragEvent evt(rel);
    dragging->dispatch_event(&evt);
  }
  GPWidget *gp = WindowManager::INSTANCE->get_hovered_gp(pos);
  if (gp == cur)
    return;
  if (cur) {
    Event e(MOUSE_LEAVE_EVENT);
    cur->dispatch_event(&e);
  }
  if (gp) {
    Event e(MOUSE_ENTER_EVENT);
    gp->dispatch_event(&e);
  }
  cur = gp;
}
