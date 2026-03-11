#pragma once

#include "../base/widget.hpp"

static constexpr u32 CLICK_EVENT = 200;

struct MyWidget : Widget {
  MyWidget(Widget *parent = nullptr);

  void on_drag_end(Event *);
  void on_enter(Event *);
  void on_leave(Event *);
  void on_press(MouseButtonEvent *e);
  void on_release(MouseButtonEvent *e);

  bool hovering = false;
  bool pressing = false;
  struct RectGPWidget *rect = nullptr;
};
