#pragma once

#include <glm/glm.hpp>

struct SDL_Window;

namespace ui::render {

struct MainWindow {
  SDL_Window *w;
  void *ctx;
  glm::vec2 wnd_size;

  void init(const char *name, glm::vec2 size);
  void cleanup();
  void swap();
};

} // namespace ui::render
