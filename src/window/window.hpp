#pragma once

#include "../base/widget.hpp"
#include <glm/glm.hpp>

constexpr u32 WINDOW_ACTIVATE_EVENT = 100;

struct Window : Widget {
  Window();
  ~Window() override;

  void render_at(glm::vec2 pos, CmdList &out_commands) const override;
  void lay(LayContext) override { recalc_layout(m_current_size); }
  GPWidget *get_hovered_gp(glm::vec2 pos) override;
  void calc_min_max_size() override;
  virtual void recalc_layout(glm::vec2 size);
  virtual void set_size(glm::vec2 size);
  virtual void set_pos(glm::vec2 pos) { m_pos = pos; }
  void set_content(Widget *w);

  glm::vec2 m_pos{};
  glm::vec2 m_content_offset{};
  Widget *m_content = nullptr;
  u32 m_current_resize_handle = 0;
  bool m_fit_to_content = false;
  bool m_visible = true;
  bool m_disabled = false;

  struct RectGPWidget *m_event_fallback = nullptr;
  struct RectGPWidget *m_move_handle = nullptr;
  struct RectGPWidget *m_resize_handles[8] = {};

  struct ControlWidget *m_control_list = nullptr;

  void add_control(struct ControlWidget *w);
};
