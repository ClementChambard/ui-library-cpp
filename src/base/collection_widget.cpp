#include "collection_widget.hpp"

GPWidget *CollectionWidget::get_hovered_gp(glm::vec2 pos) {
  if (pos.x < 0 || pos.y < 0)
    return nullptr;
  if (pos.x > m_current_size.x || pos.y > m_current_size.y)
    return nullptr;
  GPWidget *out = nullptr;
  for (u32 i = 0; i < get_child_count(); i++) {
    auto c_pos = get_child_relative_position(i);
    auto gp = get_child(i)->get_hovered_gp(pos - c_pos);
    if (gp)
      out = gp;
  }
  return out;
}
