#pragma once

#include "draw_batch.hpp"
#include "render_command.hpp"

struct Renderer {
  DrawBatch *b;
  CmdList cmd_list;
  glm::vec2 wnd_size;
  void init();
  void cleanup();
  void render();
  void update_size(glm::vec2 s);
};
