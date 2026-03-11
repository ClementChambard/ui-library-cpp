#pragma once

#include "widget.hpp"

struct CollectionWidgetHolder {
  Widget *widget = nullptr;
  glm::vec2 pos{};
};

struct CollectionWidget : Widget {
  CollectionWidget(Widget *parent = nullptr) : Widget(parent) {}

  struct GPWidget *get_hovered_gp(glm::vec2 pos) override;

  virtual u32 get_child_count() = 0;
  virtual Widget *get_child(u32 i) = 0;
  virtual glm::vec2 get_child_relative_position(u32 i) = 0;
};
