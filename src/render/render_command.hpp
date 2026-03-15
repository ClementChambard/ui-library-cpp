#pragma once

#include "../base/color.hpp"
#include "../base/font.hpp"
#include "../defines.hpp"
#include <glm/glm.hpp>
#include <string>

enum RenderCommand_kind {
  RenderCommand_RECT,
  RenderCommand_RECT_BORDER,
  RenderCommand_ROUND_RECT,
  RenderCommand_ROUND_RECT_BORDER,
  RenderCommand_TRIANGLE,
  RenderCommand_TEXT,
  RenderCommand_SCISSOR,
};

struct RenderCommand_rect {
  RenderCommand_kind kind = RenderCommand_RECT;
  glm::vec2 pos{}, size{};
  Color col{};
  f32 r_tl = 0.f, r_tr = 0.f, r_br = 0.f, r_bl = 0.f;
  f32 outline_size = 0.f;
};

struct RenderCommand_tri {
  RenderCommand_kind kind = RenderCommand_SCISSOR;
  glm::vec2 p1{}, p2{}, p3{};
  Color col{};
};

struct RenderCommand_scissor {
  RenderCommand_kind kind = RenderCommand_SCISSOR;
  glm::vec2 pos{}, size{};
  bool enable = false;
};

struct RenderCommand_text {
  RenderCommand_kind kind = RenderCommand_TEXT;
  glm::vec2 pos{};
  char const *text = nullptr;
  Font *font = nullptr;
  Color col;
  f32 wrap_width = -1.f;
  bool multiline = false;
};

union RenderCommand {
  RenderCommand_kind kind;
  RenderCommand_rect rect;
  RenderCommand_tri tri;
  RenderCommand_text text;
  RenderCommand_scissor scissor;
};

using CmdList = std::vector<RenderCommand>;

void draw_triangle(CmdList &cmds, glm::vec2 p1, glm::vec2 p2, glm::vec2 p3,
                   Color c);
void draw_circle(CmdList &cmds, glm::vec2 center, f32 radius, Color c);
void draw_circle_outline(CmdList &cmds, glm::vec2 center, f32 radius, Color c,
                         f32 outline_size);
void draw_rectangle(CmdList &cmds, glm::vec2 pos, glm::vec2 size, Color c);
void draw_rectangle(CmdList &cmds, glm::vec2 pos, glm::vec2 size, Color c,
                    f32 corner_radius);
void draw_rectangle(CmdList &cmds, glm::vec2 pos, glm::vec2 size, Color c,
                    f32 r_tl, f32 r_tr, f32 r_br, f32 r_bl);
void draw_rectangle_outline(CmdList &cmds, glm::vec2 pos, glm::vec2 size,
                            Color c, f32 outline_size);
void draw_rectangle_outline(CmdList &cmds, glm::vec2 pos, glm::vec2 size,
                            Color c, f32 corner_radius, f32 outline_size);
void draw_rectangle_outline(CmdList &cmds, glm::vec2 pos, glm::vec2 size,
                            Color c, f32 r_tl, f32 r_tr, f32 r_br, f32 r_bl,
                            f32 outline_size);
void draw_set_scissor(CmdList &cmds, glm::vec2 pos, glm::vec2 size);
void draw_disable_scissor(CmdList &cmds);
void draw_text(CmdList &cmds, glm::vec2 pos, std::string const &text, Color col,
               Font *font = nullptr);
void draw_text_multiline(CmdList &cmds, glm::vec2 pos, std::string const &text,
                         Color col, Font *font = nullptr);
void draw_text_wrap(CmdList &cmds, glm::vec2 pos, std::string const &text,
                    Color col, f32 wrap_width, Font *font = nullptr);
void draw_text_multiline_wrap(CmdList &cmds, glm::vec2 pos,
                              std::string const &text, Color col,
                              f32 wrap_width, Font *font = nullptr);
