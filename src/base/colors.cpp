#include "colors.hpp"

using namespace ui;

static ColorScheme::LightDark s_light_dark = ColorScheme::LIGHT;

static ColorScheme s_dark = {
    .primary =
        {
            .bg =
                {
                    .base = {110, 0, 130, 255},
                    .hover = {122, 3, 150, 255},
                    .press = {150, 10, 170, 255},

                },
            .fg =
                {
                    .base = c_white,
                    .hover = c_white,
                    .press = c_white,
                },
            .border =
                {
                    .base = {117, 10, 143, 255},
                    .hover = {130, 10, 160, 255},
                    .press = {160, 10, 180, 255},
                },
        },
    .secondary =
        {
            .bg =
                {
                    .base = {40, 40, 40, 255},
                    .hover = {50, 50, 50, 255},
                    .press = {68, 68, 68, 255},
                },
            .fg =
                {
                    .base = {232, 232, 232, 255},
                    .hover = {232, 232, 232, 255},
                    .press = {232, 232, 232, 255},
                },
            .border =
                {
                    .base = {66, 65, 64, 255},
                    .hover = {70, 70, 70, 255},
                    .press = {78, 78, 78, 255},
                },
        },
    .panel =
        {
            .topbar = {70, 70, 70, 255},
            .fg = c_white,
            .btn_hover = {88, 88, 88, 255},
        },
    .dialog = {.bg1 = {35, 35, 35, 255},
               .bg2 = {38, 38, 38, 255},
               .fg = {232, 232, 232, 255},
               .border = {66, 66, 66, 255},
               .separator = {50, 51, 52, 255}},
    .input_bg = {30, 30, 30, 255},
    .input_fg = {232, 232, 232, 255},
};

static ColorScheme s_light = {
    .primary =
        {
            .bg =
                {
                    .base = {160, 10, 184, 255},
                    .hover = {117, 5, 133, 255},
                    .press = {100, 0, 118, 255},

                },
            .fg =
                {
                    .base = c_white,
                    .hover = c_white,
                    .press = c_white,
                },
            .border =
                {
                    .base = {140, 0, 170, 255},
                    .hover = {107, 0, 123, 255},
                    .press = {100, 0, 110, 255},
                },
        },
    .secondary =
        {
            .bg =
                {
                    .base = {241, 241, 241, 255},
                    .hover = {226, 226, 226, 255},
                    .press = {209, 212, 212, 255},
                },
            .fg =
                {
                    .base = {61, 61, 61, 255},
                    .hover = {61, 61, 61, 255},
                    .press = {61, 61, 61, 255},
                },
            .border =
                {
                    .base = {180, 182, 186, 255},
                    .hover = {119, 121, 124, 255},
                    .press = {61, 61, 61, 255},
                },
        },
    .panel =
        {
            .topbar = {192, 192, 192, 255},
            .fg = c_white,
            .btn_hover = {174, 174, 174, 255},
        },
    .dialog = {.bg1 = c_white,
               .bg2 = {244, 245, 246, 255},
               .fg = {61, 61, 61, 255},
               .border = {209, 209, 209, 255},
               .separator = {226, 228, 227, 255}},
    .input_bg = {251, 251, 251, 255},
    .input_fg = {61, 61, 61, 255},
};

static ColorScheme *s_current = &s_light;

ColorScheme &ColorScheme::dark() { return s_dark; }

ColorScheme &ColorScheme::light() { return s_light; }

ColorScheme const &ColorScheme::current() { return *s_current; }

void ColorScheme::use(LightDark kind) {
  s_light_dark = kind;
  s_current = s_light_dark == LIGHT ? &s_light : &s_dark;
}

ColorScheme::LightDark ColorScheme::current_kind() { return s_light_dark; }
