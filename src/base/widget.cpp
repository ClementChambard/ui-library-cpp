#include "widget.hpp"

Widget::Widget(Widget *parent) : m_parent(parent) {
  if (parent)
    parent->_append(this);
}

Widget::~Widget() {
  if (m_internal_widget)
    delete m_internal_widget;
  for (auto el : m_event_listeners)
    delete el;
};

void Widget::dispatch_event(Event *e) {
  if (e->origin == nullptr) {
    e->origin = this;
  }
  for (auto const &el : m_event_listeners) {
    if (el->id != e->id)
      continue;
    if (el->who != nullptr && el->who != e->origin)
      continue;
    if (e->consumed && !el->handles_consumed_events)
      continue;
    el->apply(e, this);
  }
  if (m_parent)
    m_parent->dispatch_event(e);
}

void Widget::render_at(glm::vec2 pos, CmdList &out_commands) const {
  if (m_internal_widget)
    return m_internal_widget->render_at(pos, out_commands);
}

void Widget::lay(glm::vec2 min_size, glm::vec2 max_size) {
  if (m_internal_widget)
    return m_internal_widget->lay(min_size, max_size);
}

struct GPWidget *Widget::get_hovered_gp(glm::vec2 pos) {
  return m_internal_widget ? m_internal_widget->get_hovered_gp(pos) : nullptr;
}
