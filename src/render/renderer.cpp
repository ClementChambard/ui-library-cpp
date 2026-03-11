#include "renderer.hpp"
#include "draw_batch.hpp"
#include <GL/glew.h>

void Renderer::init() {
  glewInit();
  // glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glBlendEquation(GL_ADD);
  b = new DrawBatch;
}

void Renderer::cleanup() { delete b; }

void Renderer::update_size(glm::vec2 s) {
  wnd_size = s;
  b->update_wnd_size(s);
  glViewport(0, 0, s.x, s.y);
}

void Renderer::render() {
  glClearColor(1, 1, 1, 1);
  glClearDepth(1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  for (auto const &c : cmd_list) {
    if (c.kind == RenderCommand_RECT) {
      b->draw_rectangle(c.rect.pos, c.rect.size, c.rect.col);
    } else if (c.kind == RenderCommand_RECT_BORDER) {
      b->draw_rectangle_outline(c.rect.pos, c.rect.size, c.rect.col,
                                c.rect.outline_size);
    } else if (c.kind == RenderCommand_ROUND_RECT) {
      b->draw_round_rectangle(c.rect.pos, c.rect.size, c.rect.col, c.rect.r_tl);
    } else if (c.kind == RenderCommand_ROUND_RECT_BORDER) {
      b->draw_round_rectangle_outline(c.rect.pos, c.rect.size, c.rect.col,
                                      c.rect.outline_size, c.rect.r_tl);
    } else if (c.kind == RenderCommand_TRIANGLE) {
      b->draw_triangle(c.tri.p1, c.tri.p2, c.tri.p3, c.tri.col);
    } else if (c.kind == RenderCommand_SCISSOR) {
      b->submit();
      if (c.scissor.enable) {
        glEnable(GL_SCISSOR_TEST);
        glScissor(c.scissor.pos.x,
                  wnd_size.y - c.scissor.pos.y - c.scissor.size.y,
                  c.scissor.size.x, c.scissor.size.y);
      } else {
        glDisable(GL_SCISSOR_TEST);
      }
    }
  }
  b->submit();
}
