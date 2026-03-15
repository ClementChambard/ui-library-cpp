#pragma once

#include "../base/widget.hpp"

enum class Key {
  ESCAPE,
  RETURN,
  DELETE,
  BACKSPACE,
  LEFT,
  RIGHT,
  UP,
  DOWN,
  UNKNOWN,
};

struct ControlWidget : Widget {
  ControlWidget(Widget *parent);
  ~ControlWidget() override;

  bool m_focused = false;
  bool m_disabled = false;
  bool m_need_text_input = false;

  bool is_disabled() const;

  void lay(LayContext ctx) override;
  void set_disabled(bool val) { m_disabled = val; }

  // TODO: k should be enum with allowed keys
  virtual void on_key(Key k) { (void)k; }
  virtual void on_text(char const *text) { (void)text; }

  ControlWidget *m_next_control_in_window = nullptr;
  ControlWidget *m_prev_control_in_window = nullptr;
  struct Window *m_window = nullptr;
};
