#pragma once

#include "../defines.hpp"
#include <glm/glm.hpp>

namespace ui {

using Color = glm::vec<4, u8>;

static constexpr Color c_black = {0, 0, 0, 255};
static constexpr Color c_white = {255, 255, 255, 255};
static constexpr Color c_transparent = {};

struct ColorSet {
  Color base, hover, press;
};

struct ContainerColorSet {
  ColorSet bg, fg, border;
};

struct ContainerColors {
  Color bg, fg, border;
};

struct ColorScheme {
  ContainerColorSet primary;
  ContainerColorSet secondary;
  struct {
    Color topbar;
    Color fg;
    Color btn_hover;
  } panel;
  struct {
    Color bg1;
    Color bg2;
    Color fg;
    Color border;
    Color separator;
  } dialog;
  Color input_bg;
  Color input_fg;

  static ColorScheme &dark();
  static ColorScheme &light();
  static ColorScheme const &current();

  enum LightDark {
    DARK,
    LIGHT,
  };
  static void use(LightDark kind);
  static LightDark current_kind();
};

} // namespace ui
