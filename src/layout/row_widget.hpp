#pragma once

#include "../base/collection_widget.hpp"

struct RowWidget : CollectionWidget<> {
  RowWidget(Widget *parent = nullptr) : CollectionWidget(parent) {}
  ~RowWidget() = default;

  virtual void render_at(glm::vec2 pos, CmdList &out_commands) const override;
  virtual void lay(LayContext ctx) override;
  virtual void calc_min_max_size() override;

  f32 m_gap = 0.f;
};
