#include "render_command.hpp"
#include <cstring>

void draw_rectangle(CmdList &cmds, glm::vec2 pos, glm::vec2 size, Color c) {
  if (size.x < 0 || size.y < 0)
    return;
  cmds.push_back({.rect = {
                      .kind = RenderCommand_RECT,
                      .pos = pos,
                      .size = size,
                      .col = c,
                  }});
}
void draw_rectangle(CmdList &cmds, glm::vec2 pos, glm::vec2 size, Color c,
                    f32 corner_radius) {
  draw_rectangle(cmds, pos, size, c, corner_radius, corner_radius,
                 corner_radius, corner_radius);
}
void draw_rectangle(CmdList &cmds, glm::vec2 pos, glm::vec2 size, Color c,
                    f32 r_tl, f32 r_tr, f32 r_br, f32 r_bl) {
  if (size.x < 0 || size.y < 0)
    return;
  cmds.push_back({.rect = {
                      .kind = RenderCommand_ROUND_RECT,
                      .pos = pos,
                      .size = size,
                      .col = c,
                      .r_tl = r_tl,
                      .r_tr = r_tr,
                      .r_br = r_br,
                      .r_bl = r_bl,
                  }});
}
void draw_rectangle_outline(CmdList &cmds, glm::vec2 pos, glm::vec2 size,
                            Color c, f32 outline_size) {
  if (size.x < 0 || size.y < 0)
    return;
  cmds.push_back({.rect = {
                      .kind = RenderCommand_RECT_BORDER,
                      .pos = pos,
                      .size = size,
                      .col = c,
                      .outline_size = outline_size,
                  }});
}
void draw_rectangle_outline(CmdList &cmds, glm::vec2 pos, glm::vec2 size,
                            Color c, f32 corner_radius, f32 outline_size) {
  draw_rectangle_outline(cmds, pos, size, c, corner_radius, corner_radius,
                         corner_radius, corner_radius, outline_size);
}
void draw_rectangle_outline(CmdList &cmds, glm::vec2 pos, glm::vec2 size,
                            Color c, f32 r_tl, f32 r_tr, f32 r_br, f32 r_bl,
                            f32 outline_size) {
  if (size.x < 0 || size.y < 0)
    return;
  cmds.push_back({.rect = {
                      .kind = RenderCommand_ROUND_RECT_BORDER,
                      .pos = pos,
                      .size = size,
                      .col = c,
                      .r_tl = r_tl,
                      .r_tr = r_tr,
                      .r_br = r_br,
                      .r_bl = r_bl,
                      .outline_size = outline_size,
                  }});
}
void draw_set_scissor(CmdList &cmds, glm::vec2 pos, glm::vec2 size) {
  if (size.x < 0 || size.y < 0) {
    size.x = size.y = 0;
  }
  cmds.push_back({.scissor = {
                      .kind = RenderCommand_SCISSOR,
                      .pos = pos,
                      .size = size,
                      .enable = true,
                  }});
}
void draw_disable_scissor(CmdList &cmds) {
  cmds.push_back({.scissor = {
                      .kind = RenderCommand_SCISSOR,
                      .pos = {},
                      .size = {},
                      .enable = false,
                  }});
}

void draw_triangle(CmdList &cmds, glm::vec2 p1, glm::vec2 p2, glm::vec2 p3,
                   Color c) {
  cmds.push_back({.tri = {
                      .kind = RenderCommand_TRIANGLE,
                      .p1 = p1,
                      .p2 = p2,
                      .p3 = p3,
                      .col = c,
                  }});
}

void draw_text(CmdList &cmds, glm::vec2 pos, std::string const &text,
               Font *font) {
  cmds.push_back({.text = {
                      .kind = RenderCommand_TEXT,
                      .pos = pos,
                      .text = strdup(text.c_str()),
                      .font = font,
                      .multiline = false,
                      .wrap_width = -1.f,
                  }});
}
void draw_text_multiline(CmdList &cmds, glm::vec2 pos, std::string const &text,
                         Font *font) {
  draw_text(cmds, pos, text, font);
  cmds.back().text.multiline = true;
}
void draw_text_wrap(CmdList &cmds, glm::vec2 pos, std::string const &text,
                    f32 wrap_width, Font *font) {
  draw_text(cmds, pos, text, font);
  cmds.back().text.wrap_width = wrap_width;
}
void draw_text_multiline_wrap(CmdList &cmds, glm::vec2 pos,
                              std::string const &text, f32 wrap_width,
                              Font *font) {
  draw_text(cmds, pos, text, font);
  cmds.back().text.wrap_width = wrap_width;
  cmds.back().text.multiline = true;
}
