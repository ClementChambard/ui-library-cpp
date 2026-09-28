#include "ma_rect.hpp"

using namespace ui;

MouseArea *MARect::check(glm::vec2 pt, glm::vec2 *out_pos) {
  auto res = (pt.x > 0 && pt.x <= m_size.x && pt.y > 0 && pt.y <= m_size.y);
  if (res && out_pos) {
    *out_pos = pt;
  }
  return res ? this : nullptr;
}
