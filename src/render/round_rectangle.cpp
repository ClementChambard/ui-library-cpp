#include "draw_batch.hpp"
#include "draw_batch_state.hpp"
#include <cassert>
#include <glm/fwd.hpp>

static constexpr glm::vec2 UV0 = {0, 0};

void unstrip_indices(DrawBatchState &b, std::vector<u16> &&strip) {
  if (strip.size() == 0)
    return;
  assert(strip.size() >= 3);
  u32 last1 = strip[0];
  u32 last2 = strip[1];
  bool pair = true;
  for (u64 i = 2; i < strip.size(); i++) {
    u32 cur = strip[i];
    b.indices.push_back(last1);
    if (pair) {
      b.indices.push_back(last2);
      b.indices.push_back(cur);
    } else {
      b.indices.push_back(cur);
      b.indices.push_back(last2);
    }
    last1 = last2;
    last2 = cur;
    pair = !pair;
  }
}

void arc_inner_vertices(std::vector<vertex_t> &vertices, glm::vec2 pos,
                        Color col, u32 n_point_needed, f32 fst_angle, f32 angle,
                        f32 radius) {
  f32 incr = angle / (n_point_needed + 1.f);
  for (u32 i = 0; i < n_point_needed; i++) {
    f32 a_pos = incr * (1 + i);
    f32 a = fst_angle - a_pos;
    glm::vec2 p = pos + glm::vec2(std::cos(a), -std::sin(a)) * radius;
    vertices.push_back({p, UV0, col});
  }
}

u32 round_rectangle_vertices(std::vector<vertex_t> &vertices, glm::vec2 pos,
                             glm::vec2 size, Color col, f32 radius,
                             u32 &o_first_index, u32 &o_last_index,
                             u32 n_point_needed = 0) {
  if (size.x < radius * 2)
    radius = size.x / 2;
  if (size.y < radius * 2)
    radius = size.y / 2;
  o_first_index = vertices.size();

  f32 x1 = pos.x + radius;
  f32 x2 = pos.x + size.x - radius;
  f32 y1 = pos.y + radius;
  f32 y2 = pos.y + size.y - radius;
  if (n_point_needed == 0)
    n_point_needed = std::numbers::pi / (4.0f * std::acos(1 - 0.33 / radius));
  bool x_degenerate = x1 == x2;
  bool y_degenerate = y1 == y2;

  u32 n_vertex = n_point_needed * 4 + 4;
  if (x_degenerate)
    n_vertex += 2;
  if (y_degenerate)
    n_vertex += 2;

  vertices.reserve(n_vertex);

  vertices.push_back({{pos.x, y1}, UV0, col});
  arc_inner_vertices(vertices, {x1, y1}, col, n_point_needed, std::numbers::pi,
                     std::numbers::pi / 2.f, radius);
  vertices.push_back({{x1, pos.y}, UV0, col});
  if (!x_degenerate)
    vertices.push_back({{x2, pos.y}, UV0, col});
  arc_inner_vertices(vertices, {x2, y1}, col, n_point_needed,
                     std::numbers::pi / 2.f, std::numbers::pi / 2.f, radius);
  vertices.push_back({{pos.x + size.x, y1}, UV0, col});
  if (!y_degenerate)
    vertices.push_back({{pos.x + size.x, y2}, UV0, col});
  arc_inner_vertices(vertices, {x2, y2}, col, n_point_needed, 0,
                     std::numbers::pi / 2.f, radius);
  vertices.push_back({{x2, pos.y + size.y}, UV0, col});
  if (!x_degenerate)
    vertices.push_back({{x1, pos.y + size.y}, UV0, col});
  arc_inner_vertices(vertices, {x1, y2}, col, n_point_needed,
                     -std::numbers::pi / 2.f, std::numbers::pi / 2.f, radius);
  if (!y_degenerate)
    vertices.push_back({{pos.x, y2}, UV0, col});

  o_last_index = vertices.size();
  return n_point_needed;
}

void concave_polygon_triangle_fan(std::vector<u16> &indices, u32 first_index,
                                  u32 last_index) {
  u32 count = last_index - first_index;
  if (count < 3)
    return;
  indices.reserve(indices.size() + (count - 2) * 3);
  u32 c = first_index;
  u32 p1 = first_index + 1;
  for (u32 i = first_index + 2; i < last_index; i++) {
    indices.push_back(c);
    indices.push_back(p1);
    indices.push_back(i);
    p1 = i;
  }
}

void link_lines_strip(std::vector<u16> &indices, u32 first_index_1,
                      u32 first_index_2, u32 count, bool wraparound = false) {
  if (count < 2)
    return;
  u32 last_1 = first_index_1, last_2 = first_index_2;
  for (u32 i = 1; i < count; i++) {
    u32 i_1 = first_index_1 + i;
    u32 i_2 = first_index_2 + i;
    indices.push_back(last_1);
    indices.push_back(i_1);
    indices.push_back(i_2);
    indices.push_back(last_1);
    indices.push_back(i_2);
    indices.push_back(last_2);
    last_1 = i_1;
    last_2 = i_2;
  }
  if (wraparound && count > 2) {
    indices.push_back(last_1);
    indices.push_back(first_index_1);
    indices.push_back(first_index_2);
    indices.push_back(last_1);
    indices.push_back(first_index_2);
    indices.push_back(last_2);
  }
}

void DrawBatch::draw_round_rectangle(glm::vec2 pos, glm::vec2 size, Color c,
                                     f32 radius) {
  auto s = static_cast<DrawBatchState *>(state);
  if (s->cur_tex != s->default_tex) {
    submit();
    s->use_texture(s->default_tex);
  }
  u32 first_index, last_index;
  round_rectangle_vertices(s->vertices, pos, size, c, radius, first_index,
                           last_index);
  concave_polygon_triangle_fan(s->indices, first_index, last_index);
}

void DrawBatch::draw_round_rectangle_outline(glm::vec2 pos, glm::vec2 size,
                                             Color c, f32 outline_size,
                                             f32 radius) {
  if (size.x < outline_size * 2 || size.y < outline_size * 2)
    return draw_round_rectangle(pos, size, c, radius);
  auto s = static_cast<DrawBatchState *>(state);
  if (s->cur_tex != s->default_tex) {
    submit();
    s->use_texture(s->default_tex);
  }
  u32 first_index_out, last_index_out;
  u32 first_index_in, last_index_in;
  u32 n_point = round_rectangle_vertices(s->vertices, pos, size, c, radius,
                                         first_index_out, last_index_out);
  round_rectangle_vertices(
      s->vertices, pos + glm::vec2(outline_size, outline_size),
      size - 2.f * glm::vec2(outline_size, outline_size), c,
      radius - outline_size, first_index_in, last_index_in, n_point);
  u32 count = last_index_out - first_index_out;
  assert(count == last_index_in - first_index_in);
  link_lines_strip(s->indices, first_index_out, first_index_in, count, true);
}

void DrawBatch::draw_textured_rectangle(u32 tex, glm::vec2 pos, glm::vec2 size,
                                        glm::vec2 uv1, glm::vec2 uv2, Color c) {
  auto s = static_cast<DrawBatchState *>(state);
  if (s->cur_tex != tex) {
    submit();
    s->use_texture(tex);
  }
  u32 tl = s->vertex({pos, uv1, c});
  u32 tr = s->vertex({{pos.x + size.x, pos.y}, {uv2.x, uv1.y}, c});
  u32 bl = s->vertex({{pos.x, pos.y + size.y}, {uv1.x, uv2.y}, c});
  u32 br = s->vertex({pos + size, uv2, c});

  s->quad(tl, tr, br, bl);
}
