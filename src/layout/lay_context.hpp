#pragma once

#include <glm/glm.hpp>

struct LayContext {
  struct Window *current_window;
  glm::vec2 min_size;
  glm::vec2 max_size;
};
