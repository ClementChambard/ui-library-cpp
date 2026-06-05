#pragma once

#include "../defines.hpp"

namespace ui::render {

static constexpr f64 FPS = 60.0;

void frame_timer_start();
void frame_timer_end();

} // namespace ui::render
