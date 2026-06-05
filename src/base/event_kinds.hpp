#pragma once

#include "../defines.hpp"

namespace ui::event {

using Kind = u32;

static constexpr Kind EVENT_INVALID = 0;

// Mouse Events
static constexpr Kind MOUSE_ENTER = 1;
static constexpr Kind MOUSE_LEAVE = 2;
static constexpr Kind MOUSE_PRESS = 3;
static constexpr Kind MOUSE_RELEASE = 4;
static constexpr Kind MOUSE_DRAG_START = 5;
static constexpr Kind MOUSE_DRAG_END = 6;
static constexpr Kind MOUSE_DRAG = 7;

// Control Events
static constexpr u32 BUTTON_CLICK = 200;
static constexpr u32 STANDARD_BUTTON_CLICK = 201;

} // namespace ui::event
