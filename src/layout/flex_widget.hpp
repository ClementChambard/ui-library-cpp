#pragma once
#include "../base/collection_widget.hpp"

namespace Flex {
enum Direction {
  HORIZONTAL,
  VERTICAL,
};
enum Alignment {
  START,
  CENTER,
  END,
  SPACE_BETWEEN,
  SPACE_AROUND,
  STRETCH,
  INHERIT,
};
struct ChildHolder : CollectionChildHolder {
  f32 flex = 0.f;
  Alignment align_self = INHERIT;
};
} // namespace Flex

struct FlexWidget : CollectionWidget<Flex::ChildHolder> {
  FlexWidget(Widget *parent = nullptr) : CollectionWidget(parent) {}
  ~FlexWidget() = default;

  void render_at(glm::vec2 pos, CmdList &out_commands) const override;
  void lay(LayContext ctx) override;
  void calc_min_max_size() override;

  void lay_w_dir(LayContext ctx, f32 glm::vec2::*main_axis,
                 f32 glm::vec2::*cross_axis);

  Flex::Direction m_direction = Flex::HORIZONTAL;
  Flex::Alignment m_main_axis_alignment = Flex::START;
  Flex::Alignment m_cross_axis_alignment = Flex::START;
  // TODO: wrapping
  f32 m_padding_top = 0.f, m_padding_right = 0.f, m_padding_left = 0.f,
      m_padding_bottom = 0.f;
  f32 m_gap = 0.f;
};
