#pragma once

#include "../base/color.hpp"
#include <glm/glm.hpp>

class DrawBatch {
  void *state;

public:
  DrawBatch();
  ~DrawBatch();
  void draw_triangle(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, Color c);
  void draw_rectangle(glm::vec2 pos, glm::vec2 size, Color c);
  void draw_rectangle_outline(glm::vec2 pos, glm::vec2 size, Color c,
                              f32 outline_size);
  void draw_round_rectangle(glm::vec2 pos, glm::vec2 size, Color c, f32 radius);
  void draw_round_rectangle_outline(glm::vec2 pos, glm::vec2 size, Color c,
                                    f32 outline_size, f32 radius);
  void draw_round_rectangle(glm::vec2 pos, glm::vec2 size, Color c, glm::vec4 radius);
  void draw_round_rectangle_outline(glm::vec2 pos, glm::vec2 size, Color c,
                                    f32 outline_size, glm::vec4 radius);
  void draw_textured_rectangle(u32 tex, glm::vec2 pos, glm::vec2 size, glm::vec2 uv1, glm::vec2 uv2, Color c);
  void draw_text(glm::vec2 pos, struct Font *f, char const *text, Color col);
  void submit();
  void update_wnd_size(glm::vec2 s);
};
