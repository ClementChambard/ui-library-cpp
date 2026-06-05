#include "ma_rect.hpp"

using namespace ui;

MouseArea *MARect::check(glm::vec2 pt) {
  return (pt.x > 0 && pt.x <= m_size.x && pt.y > 0 && pt.y <= m_size.y) ? this : nullptr;
}
