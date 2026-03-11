#include "font.hpp"

// TODO: ascii

Font Font::load(std::string const &filename) {
  // TODO:
  (void)filename;
  return {};
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
