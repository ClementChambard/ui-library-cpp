#pragma once

#include "../base/color.hpp"
#include "../defines.hpp"
#include <glm/glm.hpp>

struct Shader {
  u32 program_id = 0;
  i32 uniform_loc = 0;
  void init(const char *vtx, i32 vtx_s, const char *fgr, i32 fgr_s);
  ~Shader();
};

struct vertex_t {
  glm::vec2 pos{};
  glm::vec2 uv{};
  Color col{};
};

struct DrawBatchState {
  DrawBatchState();
  ~DrawBatchState();
  u32 vao_id = 0;
  u32 vbo_id = 0;
  u32 ibo_id = 0;
  u32 default_tex = 0;
  u32 cur_tex = 0;
  Shader shdr{};
  std::vector<vertex_t> vertices{};
  std::vector<u16> indices{};
  void triangle(vertex_t p1, vertex_t p2, vertex_t p3);
  void triangle(u32 i1, u32 i2, u32 i3);
  void quad(vertex_t p1, vertex_t p2, vertex_t p3, vertex_t p4);
  void quad(u32 i1, u32 i2, u32 i3, u32 i4);
  void use_texture(u32 tex);

  u32 vertex(vertex_t v);
  void index(u32 i);
};
