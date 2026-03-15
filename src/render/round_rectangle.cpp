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

f32 calc_point_needed(f32 radius) {
  if (radius == 0)
    return 0;
  // TODO: cache result to avoid too much trig ?
  return std::numbers::pi / (4.0f * std::acos(1 - 0.33 / radius));
}

glm::vec4 calc_point_needed(glm::vec4 radius) {
  return {
      calc_point_needed(radius.x),
      calc_point_needed(radius.y),
      calc_point_needed(radius.z),
      calc_point_needed(radius.w),
  };
}

void round_rectangle_vertices(std::vector<vertex_t> &vertices, glm::vec2 pos,
                              glm::vec2 size, Color col, glm::vec4 radius,
                              u32 &o_first_index, u32 &o_last_index,
                              glm::vec4 n_point_needed) {
  o_first_index = vertices.size();

  glm::vec2 tl_inner = pos + glm::vec2(radius.x, radius.x);
  glm::vec2 tr_inner = pos + glm::vec2(size.x - radius.y, radius.y);
  glm::vec2 br_inner = pos + size - glm::vec2(radius.z, radius.z);
  glm::vec2 bl_inner = pos + glm::vec2(radius.w, size.y - radius.w);

  // TODO: move these points and change radius in case of them being in the
  // wrong place if (size.x < radius.x * 2)
  //   radius.x = size.x / 2;
  // if (size.y < radius.x * 2)
  //   radius.x = size.y / 2;

  bool l_degenerate = tl_inner.y == bl_inner.y;
  bool t_degenerate = tl_inner.x == tr_inner.x;
  bool r_degenerate = tr_inner.y == br_inner.y;
  bool b_degenerate = bl_inner.x == br_inner.x;

  u32 n_vertex = n_point_needed.x + n_point_needed.y + n_point_needed.z +
                 n_point_needed.w + 4;
  if (l_degenerate)
    n_vertex++;
  if (t_degenerate)
    n_vertex++;
  if (r_degenerate)
    n_vertex++;
  if (b_degenerate)
    n_vertex++;

  vertices.reserve(n_vertex);

  if (radius.x != 0) {
    vertices.push_back({{pos.x, tl_inner.y}, UV0, col});
    arc_inner_vertices(vertices, tl_inner, col, n_point_needed.x,
                       std::numbers::pi, std::numbers::pi / 2.f, radius.x);
    vertices.push_back({{tl_inner.x, pos.y}, UV0, col});
  } else {
    vertices.push_back({pos, UV0, col});
  }

  if (radius.y != 0) {
    if (!t_degenerate)
      vertices.push_back({{tr_inner.x, pos.y}, UV0, col});
    arc_inner_vertices(vertices, tr_inner, col, n_point_needed.y,
                       std::numbers::pi / 2.f, std::numbers::pi / 2.f,
                       radius.y);
    vertices.push_back({{pos.x + size.x, tr_inner.y}, UV0, col});
  } else {
    vertices.push_back({{pos.x + size.x, pos.y}, UV0, col});
  }

  if (radius.z != 0) {
    if (!r_degenerate)
      vertices.push_back({{pos.x + size.x, br_inner.y}, UV0, col});
    arc_inner_vertices(vertices, br_inner, col, n_point_needed.z, 0,
                       std::numbers::pi / 2.f, radius.z);
    vertices.push_back({{br_inner.x, pos.y + size.y}, UV0, col});
  } else {
    vertices.push_back({pos + size, UV0, col});
  }

  if (radius.w != 0) {
    if (!b_degenerate)
      vertices.push_back({{bl_inner.x, pos.y + size.y}, UV0, col});
    arc_inner_vertices(vertices, bl_inner, col, n_point_needed.w,
                       -std::numbers::pi / 2.f, std::numbers::pi / 2.f,
                       radius.w);
    if (!l_degenerate)
      vertices.push_back({{pos.x, bl_inner.y}, UV0, col});
  } else {
    vertices.push_back({{pos.x, pos.y + size.y}, UV0, col});
  }

  o_last_index = vertices.size();
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
  draw_round_rectangle(pos, size, c, {radius, radius, radius, radius});
}

void DrawBatch::draw_round_rectangle(glm::vec2 pos, glm::vec2 size, Color c,
                                     glm::vec4 radius) {
  auto s = static_cast<DrawBatchState *>(state);
  if (s->cur_tex != s->default_tex) {
    submit();
    s->use_texture(s->default_tex);
  }
  u32 first_index, last_index;

  auto points_needed = calc_point_needed(radius);
  round_rectangle_vertices(s->vertices, pos, size, c, radius, first_index,
                           last_index, points_needed);
  concave_polygon_triangle_fan(s->indices, first_index, last_index);
}

void DrawBatch::draw_round_rectangle_outline(glm::vec2 pos, glm::vec2 size,
                                             Color c, f32 outline_size,
                                             f32 radius) {
  draw_round_rectangle_outline(pos, size, c, outline_size,
                               {radius, radius, radius, radius});
}

void DrawBatch::draw_round_rectangle_outline(glm::vec2 pos, glm::vec2 size,
                                             Color c, f32 outline_size,
                                             glm::vec4 radius) {
  if (size.x < outline_size * 2 || size.y < outline_size * 2)
    return draw_round_rectangle(pos, size, c, radius);
  auto s = static_cast<DrawBatchState *>(state);
  if (s->cur_tex != s->default_tex) {
    submit();
    s->use_texture(s->default_tex);
  }
  u32 first_index_out, last_index_out;
  u32 first_index_in, last_index_in;

  auto points_needed = calc_point_needed(radius);
  round_rectangle_vertices(s->vertices, pos, size, c, radius, first_index_out,
                           last_index_out, points_needed);
  round_rectangle_vertices(
      s->vertices, pos + glm::vec2(outline_size, outline_size),
      size - 2.f * glm::vec2(outline_size, outline_size), c,
      radius - outline_size, first_index_in, last_index_in, points_needed);
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
  // std::cout << pos.x << "," << pos.y << " " << size.x << "x" << size.y << "
  // =   "; std::cout << uv1.x << "," << uv1.y << " " << uv2.x << "," << uv2.y
  // << '\n';
  u32 tl = s->vertex({pos, uv1, c});
  u32 tr = s->vertex({{pos.x + size.x, pos.y}, {uv2.x, uv1.y}, c});
  u32 bl = s->vertex({{pos.x, pos.y + size.y}, {uv1.x, uv2.y}, c});
  u32 br = s->vertex({pos + size, uv2, c});

  s->quad(tl, tr, br, bl);
}
