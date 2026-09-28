#include "text_input.hpp"
#include "../window/window_manager.hpp"

using namespace ui;

static constexpr f32 TEXTINPUT_ROUNDING = 4.f;
static constexpr f32 TEXTINPUT_OUTLINE_WIDTH = 1;
static constexpr f32 TEXTINPUT_PADDING_X = 14;
static constexpr f32 TEXTINPUT_PADDING_y = 2;
static constexpr glm::vec2 TEXTINPUT_MIN_SIZE = {32, 32};

glm::vec2 get_size(TextInput const &b) {
  glm::vec2 inner_size =
      b.m_font->calc_string_size(b.m_controller.m_text) * 2.f;
  glm::vec2 wanted_size =
      inner_size + glm::vec2(TEXTINPUT_PADDING_X + TEXTINPUT_OUTLINE_WIDTH,
                             TEXTINPUT_PADDING_y + TEXTINPUT_OUTLINE_WIDTH) *
                       2.f;
  wanted_size = glm::max(TEXTINPUT_MIN_SIZE, wanted_size);
  return wanted_size;
}

TextInput::TextInput(Widget *parent) : ControlWidget(parent) {
  m_pick_rect.set_size({200, 80});
  m_font = Font::DEFAULT;

  m_need_text_input = true;

  add_event_listener(event::MOUSE_PRESS, this, [](Event *e, TextInput *w) {
    if (!w->is_focused())
      WindowManager::INSTANCE->focus(w);
    w->m_controller.mouse_event(e);
  });
  auto send_me = [](Event *e, TextInput *w) { w->m_controller.mouse_event(e); };
  add_event_listener(event::MOUSE_DRAG_START, this, send_me);
  add_event_listener(event::MOUSE_DRAG_END, this, send_me);
  add_event_listener(event::MOUSE_DRAG, this, send_me);

  set_min_max_size(get_size(*this), {10000, 10000});
}

void TextInput::set_text(std::string const &t) {
  m_controller.set_text(t);
  auto size = get_size(*this);
  set_min_max_size(size, {10000, 10000});
}

void TextInput::render_at(glm::vec2 pos, CmdList &out_commands) const {
  glm::vec2 size = m_current_size;

  Color bg_color = ColorScheme::current().input_bg;
  Color outline_color = ColorScheme::current().secondary.border.base;
  Color text_color = ColorScheme::current().input_fg;
  if (is_disabled()) {
    bg_color.a = 128;
    outline_color.a = 128;
    text_color.a = 128;
  }
  draw_rectangle(out_commands, pos, size, bg_color, TEXTINPUT_ROUNDING);

  f32 outline_size = TEXTINPUT_OUTLINE_WIDTH;
  if (is_focused()) {
    outline_size += 1.f;
  }
  draw_rectangle_outline(out_commands, pos, size, outline_color,
                         TEXTINPUT_ROUNDING, outline_size);

  m_controller.render_text(pos, out_commands, m_font, text_color);
}

void TextInput::on_key(Key k) {
  if (m_controller.on_key(k, this)) {
    auto size = get_size(*this);
    set_min_max_size(size, {10000, 10000});
  }
}

void TextInput::on_text(char const *text) {
  if (m_controller.on_text(text, this)) {
    auto size = get_size(*this);
    set_min_max_size(size, {10000, 10000});
  }
}

void TextInput::lay(LayContext ctx) {
  ControlWidget::lay(ctx);
  auto wanted_size = get_size(*this);

  m_current_size = glm::max(wanted_size, ctx.min_size);

  m_pick_rect.set_size(m_current_size);
  m_controller.m_rect = m_current_size;
}
