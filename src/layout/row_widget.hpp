#pragma once

#include "../base/collection_widget.hpp"

struct RowWidget : CollectionWidget {
  RowWidget(Widget *parent = nullptr) : CollectionWidget(parent) {}
  ~RowWidget() override {
    for (auto const &c : m_children)
      delete c.widget;
  }

  virtual void render_at(glm::vec2 pos, CmdList &out_commands) const override;
  virtual void lay(glm::vec2 min_size, glm::vec2 max_size) override;

  u32 get_child_count() override { return m_children.size(); }
  Widget *get_child(u32 i) override { return m_children[i].widget; }
  glm::vec2 get_child_relative_position(u32 i) override {
    return m_children[i].pos;
  }

  void _append(Widget *w) override {
    m_children.push_back({w, {0, 0}});
    // TODO: request re-layout
  }

  std::vector<CollectionWidgetHolder> m_children{};
};
