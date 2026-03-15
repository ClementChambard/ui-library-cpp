#pragma once
#include "button.hpp"

static constexpr u32 STANDARD_BUTTON_CLICK_EVENT = 201;

struct StandardButtonClickEvent : Event {
  StandardButtonClickEvent(u32 std_id)
      : Event(STANDARD_BUTTON_CLICK_EVENT), std_id(std_id) {}
  u32 std_id;
};

struct StandardButtons : Widget {
  StandardButtons(Widget *parent);
  ~StandardButtons() override;

  void render_at(glm::vec2 pos, CmdList &out_commands) const override;
  void lay(LayContext ctx) override;
  struct GPWidget *get_hovered_gp(glm::vec2 pos) override;
  void calc_min_max_size() override;

  void set_mask(u32 v);

private:
  static constexpr u32 BUTTON_COUNT = 2;
  u32 m_standard_button_mask = 0;
  Button *m_standard_buttons[BUTTON_COUNT];

  void create_buttons();
};
