#pragma once

#include "window.hpp"

struct Panel : Window {
  Panel(Widget *parent = nullptr);
  ~Panel();

  void render_at(glm::vec2 pos, CmdList &out_commands) const override;
  void recalc_layout() override;
  GPWidget *get_hovered_gp(glm::vec2 pos) override;

  bool m_collapsed = false;
  struct RectGPWidget *m_topbar_rect = nullptr;
  struct RectGPWidget *m_background_rect = nullptr;
  struct PolygonGPWidget *m_resize_handle = nullptr;
  struct RectGPWidget *m_outline_rect = nullptr;
  struct CircleGPWidget *m_collapse_circle_bg = nullptr;
  struct PolygonGPWidget *m_collapse_arrow = nullptr;
};
