#pragma once

#include "../base/widget.hpp"
#include <glm/glm.hpp>

constexpr u32 WINDOW_ACTIVATE_EVENT = 100;

struct Window : Widget {
  Window(Widget *parent = nullptr) : Widget(parent) {}
  ~Window() override { delete m_content; }

  void render_at(glm::vec2 pos, CmdList &out_commands) const override;
  void lay(glm::vec2, glm::vec2) override { recalc_layout(); }
  GPWidget *get_hovered_gp(glm::vec2 pos) override;
  virtual void recalc_layout();
  virtual void set_size(glm::vec2 size);
  virtual void set_pos(glm::vec2 pos) { m_pos = pos; }
  void set_content(Widget *w) { m_content = w; }

  glm::vec2 m_pos{};
  glm::vec2 m_min_size = {8, 8};
  Widget *m_content = nullptr;
};
