#include "button.hpp"
#include "../gp/gp_rect.hpp"

static constexpr Color BASE_COL = {0, 0, 255, 255};
static constexpr Color HOVER_COL = {255, 0, 0, 255};
static constexpr Color PRESS_COL = {0, 255, 0, 255};

MyWidget::MyWidget(Widget *parent) : Widget(parent) {
  m_internal_widget = rect = new RectGPWidget(this, {200, 80}, BASE_COL);

  add_event_listener(MOUSE_ENTER_EVENT, rect, &MyWidget::on_enter);
  add_event_listener(MOUSE_LEAVE_EVENT, rect, &MyWidget::on_leave);
  add_event_listener<MouseButtonEvent>(MOUSE_PRESS_EVENT, rect,
                                       &MyWidget::on_press);
  add_event_listener<MouseButtonEvent>(MOUSE_RELEASE_EVENT, rect,
                                       &MyWidget::on_release);
  add_event_listener(MOUSE_DRAG_END_EVENT, rect, &MyWidget::on_drag_end);
}

void MyWidget::on_drag_end(Event *) {
  rect->m_color = hovering ? HOVER_COL : BASE_COL;
  pressing = false;
}

void MyWidget::on_enter(Event *) {
  if (!pressing)
    rect->m_color = HOVER_COL;
  hovering = true;
}

void MyWidget::on_leave(Event *) {
  if (!pressing)
    rect->m_color = BASE_COL;
  hovering = false;
}

void MyWidget::on_press(MouseButtonEvent *e) {
  if (e->button_id != MB_LEFT)
    return;
  rect->m_color = PRESS_COL;
  pressing = true;
}

void MyWidget::on_release(MouseButtonEvent *e) {
  if (e->button_id != MB_LEFT)
    return;
  if (!pressing)
    return;
  pressing = false;
  rect->m_color = HOVER_COL;
  auto ev = Event(CLICK_EVENT);
  dispatch_event(&ev);
}
