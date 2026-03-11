#pragma once

#include "../defines.hpp"
#include <glm/glm.hpp>

using Color = glm::vec<4, u8>;

static constexpr Color c_black = {0, 0, 0, 255};
static constexpr Color c_white = {255, 255, 255, 255};
static constexpr Color c_transparent = {};
