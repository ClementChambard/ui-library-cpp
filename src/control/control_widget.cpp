#include "control_widget.hpp"
#include "../window/window.hpp"

ControlWidget::ControlWidget(Widget *parent) : Widget(parent) {}

ControlWidget::~ControlWidget() {
  if (m_next_control_in_window == nullptr)
    return;
  if (m_next_control_in_window == this) {
    if (m_window != nullptr && m_window->m_control_list == this) {
      m_window->m_control_list = nullptr;
    }
    return;
  }
  if (m_window != nullptr && m_window->m_control_list == this) {
    m_window->m_control_list = m_next_control_in_window;
  }
  m_next_control_in_window->m_prev_control_in_window = m_prev_control_in_window;
  m_prev_control_in_window->m_next_control_in_window = m_next_control_in_window;
}

void ControlWidget::lay(LayContext ctx) {
  ctx.current_window->add_control(this);
  m_window = ctx.current_window;
}

bool ControlWidget::is_disabled() const {
  return m_disabled || (m_window != nullptr && m_window->m_disabled);
}
