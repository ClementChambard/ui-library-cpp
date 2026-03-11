#pragma once

#include "../base/collection_widget.hpp"
#include "window.hpp"

struct WindowCollection : CollectionWidget {
  WindowCollection();
  ~WindowCollection();

  u32 get_child_count() override { return m_windows.size(); }
  Widget *get_child(u32 i) override { return m_windows[i]; }
  glm::vec2 get_child_relative_position(u32) override { return {0, 0}; }
  void _append(Widget *w) override { m_windows.push_back(static_cast<Window *>(w)); }

  void render_at(glm::vec2 pos, CmdList &out_commands) const override;
  void lay(glm::vec2 min_size, glm::vec2 max_size) override;
  void on_window_activate(Event *e);

  std::vector<Window *> m_windows;
};
