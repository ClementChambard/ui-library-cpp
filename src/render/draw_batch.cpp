#include "draw_batch.hpp"
#include "draw_batch_state.hpp"
#include <GL/glew.h>
#include <cassert>

static constexpr glm::vec2 UV0 = {0, 0};

DrawBatch::DrawBatch() {
  DrawBatchState *s = new DrawBatchState;
  state = s;
}

DrawBatch::~DrawBatch() {
  DrawBatchState *s = reinterpret_cast<DrawBatchState *>(state);
  delete s;
}

void DrawBatch::update_wnd_size(glm::vec2 s) {
  DrawBatchState *st = reinterpret_cast<DrawBatchState *>(state);
  f32 matrix[] = {2.f / s.x, 0.f, 0.f,     -1.f, 0.f, -2.f / s.y, 0.f, 1.f,
                  0.f,       0.f, -0.001f, 0.f,  0.f, 0.f,        0.f, 1.f};
  glUniformMatrix4fv(st->shdr.uniform_loc, 1, GL_TRUE, matrix);
}

void DrawBatch::draw_triangle(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3,
                              Color c) {
  auto *s = reinterpret_cast<DrawBatchState *>(state);
  if (s->cur_tex != s->default_tex) {
    submit();
    s->use_texture(s->default_tex);
  }
  s->triangle({p1, UV0, c}, {p2, UV0, c}, {p3, UV0, c});
}

void DrawBatch::draw_rectangle(glm::vec2 pos, glm::vec2 size, Color c) {
  auto *s = reinterpret_cast<DrawBatchState *>(state);
  if (s->cur_tex != s->default_tex) {
    submit();
    s->use_texture(s->default_tex);
  }
  glm::vec2 tl = pos;
  glm::vec2 tr = pos + glm::vec2(size.x, 0);
  glm::vec2 br = pos + size;
  glm::vec2 bl = pos + glm::vec2(0, size.y);
  s->quad({tl, UV0, c}, {tr, UV0, c}, {br, UV0, c}, {bl, UV0, c});
}

void DrawBatch::draw_rectangle_outline(glm::vec2 pos, glm::vec2 size, Color c,
                                       f32 outline_size) {
  if (size.x < outline_size * 2 || size.y < outline_size * 2)
    return draw_rectangle(pos, size, c);
  auto *s = reinterpret_cast<DrawBatchState *>(state);
  if (s->cur_tex != s->default_tex) {
    submit();
    s->use_texture(s->default_tex);
  }
  glm::vec2 tl_pos = pos;
  glm::vec2 tr_pos = pos + glm::vec2(size.x, 0);
  glm::vec2 br_pos = pos + size;
  glm::vec2 bl_pos = pos + glm::vec2(0, size.y);

  u32 tl = s->vertex({tl_pos, UV0, c});
  u32 tr = s->vertex({tr_pos, UV0, c});
  u32 br = s->vertex({br_pos, UV0, c});
  u32 bl = s->vertex({bl_pos, UV0, c});

  u32 tl_in = s->vertex({tl_pos + glm::vec2(1, 1) * outline_size, UV0, c});
  u32 tr_in = s->vertex({tr_pos + glm::vec2(-1, 1) * outline_size, UV0, c});
  u32 br_in = s->vertex({br_pos + glm::vec2(-1, -1) * outline_size, UV0, c});
  u32 bl_in = s->vertex({bl_pos + glm::vec2(1, -1) * outline_size, UV0, c});

  s->quad(tl, tl_in, bl_in, bl);
  s->quad(tl, tr, tr_in, tl_in);
  s->quad(tr_in, tr, br, br_in);
  s->quad(bl_in, br_in, br, bl);
}

void DrawBatch::submit() {
  auto *s = reinterpret_cast<DrawBatchState *>(state);
  if (s->vertices.size() == 0 || s->indices.size() == 0)
    return;
  usize v_data_size = s->vertices.size() * sizeof(s->vertices[0]);
  usize i_data_size = s->indices.size() * sizeof(s->indices[0]);
  glNamedBufferData(s->vbo_id, v_data_size, s->vertices.data(), GL_DYNAMIC_DRAW);
  glNamedBufferData(s->ibo_id, i_data_size, s->indices.data(), GL_DYNAMIC_DRAW);
  glBindVertexArray(s->vao_id);
  glDrawElements(GL_TRIANGLES, s->indices.size(), GL_UNSIGNED_SHORT, nullptr);
  s->vertices.clear();
  s->indices.clear();
}
