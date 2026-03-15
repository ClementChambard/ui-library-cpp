#include "font.hpp"
#include <fstream>
#include <sstream>

// TODO: ascii

Font *Font::DEFAULT = nullptr;

Font Font::load(std::string const &filename) {
  Font out;
  std::ifstream f(filename);
  std::string line;
  u32 i = 0;
  while (std::getline(f, line)) {
    auto &g = out.ascii_glyphs[i];
    if (line[0] == 'x') {
      std::istringstream iss(line.substr(1));
      g.no_visuals = true;
      iss >> g.x_advance;
    } else {
      std::istringstream iss(line);
      g.no_visuals = false;
      iss >> g.x >> g.y >> g.tex_x >> g.tex_y >> g.w >> g.h >> g.x_advance;
    }
    i++;
  }
  out.tex_id = 0;
  out.tex_h = 128;
  out.tex_w = 512;
  out.line_height = 16;
  out.baseline_offset = 12;
  return out;
}

glm::vec2 Font::calc_string_size(std::string s, bool accept_newline) {
  glm::vec2 out = {0, line_height};
  if (s.size() == 0)
    return out;

  u32 cur_w = 0;

  for (u8 c : s) {
    if (c > 127)
      continue;
    cur_w += ascii_glyphs[c].x_advance;
    if (c == u8('\n') && accept_newline) {
      if (out.x < cur_w)
        out.x = cur_w;
      cur_w = 0;
      out.y += line_height;
    }
  }

  cur_w = cur_w - ascii_glyphs[(u8)s.back()].x_advance +
          ascii_glyphs[(u8)s.back()].w;
  if (out.x < cur_w)
    out.x = cur_w;

  return out;
}

u32 Font::trunc_char_count(std::string s, std::string trunc_s,
                           f32 width_available) {
  auto trunc_w = calc_string_size(trunc_s).x;
  auto w_with_trunc = width_available - trunc_w;
  u32 n = 0;
  u32 cur_w = 0;
  u32 n_to_trunc = 0;
  while (n < s.size()) {
    u8 c = s[n];
    if (c > 127)
      continue;
    u32 w = ascii_glyphs[c].x_advance;
    if (n == s.size() - 1)
      w = ascii_glyphs[c].w;
    u32 new_w = cur_w + w;
    if (new_w < w_with_trunc) {
      n_to_trunc++;
    }
    if (new_w > width_available)
      break;
    n++;
  }

  if (n == s.size())
    return n;
  return n_to_trunc;
}

glm::vec2 Font::calc_string_wrap(std::string s, f32 width_available) {
  // TODO:
  (void)s, (void)width_available;
  return {};
}

std::optional<glm::vec4> Font::get_char_uvs(char c) {
  int cp = c;
  if (!(cp >= 0 && cp <= 127))
    return {};
  auto &g = ascii_glyphs[u8(c)];
  if (g.no_visuals)
    return std::nullopt;
  f32 u1 = f32(g.tex_x) / f32(tex_w);
  f32 u2 = f32(g.tex_x + g.w) / f32(tex_w);
  f32 v1 = f32(g.tex_y) / f32(tex_h);
  f32 v2 = f32(g.tex_y + g.h) / f32(tex_h);
  return {{u1, v1, u2, v2}};
}

glm::vec4 Font::get_char_rect(char c, glm::vec2 cursor, glm::vec2 scale) {
  int cp = c;
  if (!(cp >= 0 && cp <= 127))
    return {};
  auto &g = ascii_glyphs[u8(c)];
  glm::vec2 p1 = cursor - glm::vec2(g.x, g.y) * scale;
  return {p1, g.w * scale.x, g.h * scale.y};
}

void Font::advance(char c, glm::vec2 &cursor, glm::vec2 scale) {
  int cp = c;
  if (!(cp >= 0 && cp <= 127))
    return;
  auto &g = ascii_glyphs[u8(c)];
  cursor.x += g.x_advance * scale.x;
}
