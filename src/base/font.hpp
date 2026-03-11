#pragma once

#include "../defines.hpp"
#include <glm/glm.hpp>
#include <string>

struct Font {
  u32 tex_id;
  u32 tex_w, tex_h;
  u16 line_height;
  i16 baseline_offset;
  struct Glyph {
    i16 x, y, w, h;
    u16 tex_x, tex_y;
    u16 x_advance;
    union {
      u16 flags;
      struct {
        bool no_visuals : 1;
        bool line_break_prefered_here : 1;
      };
    };
  };
  Glyph ascii_glyphs[128];

  static Font load(std::string const &filename);

  glm::vec2 calc_string_size(std::string s, bool accept_newline = false);
  u32 trunc_char_count(std::string s, std::string trunc_s, f32 width_available);
  glm::vec2 calc_string_wrap(std::string s, f32 width_available);
};
