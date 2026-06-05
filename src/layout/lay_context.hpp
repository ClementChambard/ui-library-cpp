#pragma once

#include <glm/glm.hpp>

namespace ui {

struct LayContext {
  struct Window *current_window;
  glm::vec2 min_size;
  glm::vec2 max_size;
};

} // namespace ui
