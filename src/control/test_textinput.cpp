#include "test_textinput.hpp"
#include "../gp/gp_rect.hpp"
#include "../window/window_manager.hpp"

static constexpr f32 TEXTINPUT_ROUNDING = 4.f;
static constexpr f32 TEXTINPUT_OUTLINE_WIDTH = 1;
static constexpr f32 TEXTINPUT_PADDING_X = 14;
static constexpr f32 TEXTINPUT_PADDING_y = 2;
static constexpr Color TEXTINPUT_COLOR_BG = {251, 251, 251, 255};
static constexpr Color TEXTINPUT_COLOR_OUTLINE = {180, 182, 186, 255};
static constexpr Color TEXTINPUT_COLOR_TEXT = {61, 61, 61, 255};
static constexpr glm::vec2 TEXTINPUT_MIN_SIZE = {32, 32};

glm::vec2 get_size(TestTextInput const &b) {
  glm::vec2 inner_size = b.m_font->calc_string_size(b.m_text) * 2.f;
  glm::vec2 wanted_size =
      inner_size + glm::vec2(TEXTINPUT_PADDING_X + TEXTINPUT_OUTLINE_WIDTH,
                             TEXTINPUT_PADDING_y + TEXTINPUT_OUTLINE_WIDTH) *
                       2.f;
  wanted_size = glm::max(TEXTINPUT_MIN_SIZE, wanted_size);
  return wanted_size;
}

TestTextInput::TestTextInput(Widget *parent) : ControlWidget(parent) {
  m_pick_rect = RectGPWidget::make_event_handle(this, {200, 80});
  m_font = Font::DEFAULT;

  m_need_text_input = true;

  add_event_listener(
      MOUSE_PRESS_EVENT, m_pick_rect,
      [](Event *, TestTextInput *w) { WindowManager::INSTANCE->focus(w); });

  set_min_max_size(get_size(*this), {10000, 10000});
}

TestTextInput::~TestTextInput() { delete m_pick_rect; }

void TestTextInput::set_text(std::string const &t) {
  m_text = t;
  auto size = get_size(*this);
  set_min_max_size(size, {10000, 10000});
}

void TestTextInput::render_at(glm::vec2 pos, CmdList &out_commands) const {
  glm::vec2 size = m_current_size;

  Color bg_color = TEXTINPUT_COLOR_BG;
  Color outline_color = TEXTINPUT_COLOR_OUTLINE;
  Color text_color = TEXTINPUT_COLOR_TEXT;
  if (is_disabled()) {
    bg_color.a = 128;
    outline_color.a = 128;
    text_color.a = 128;
  }
  draw_rectangle(out_commands, pos, size, bg_color, TEXTINPUT_ROUNDING);

  glm::vec2 text_size = m_font->calc_string_size(m_text) * 2.f;
  glm::vec2 offset = (size - text_size) / 2.f;

  f32 outline_size = TEXTINPUT_OUTLINE_WIDTH;
  if (m_focused) {
    outline_size += 1.f;
    static u32 GLOBAL_CURSOR_TIMER = 0;
    GLOBAL_CURSOR_TIMER++;
    if (GLOBAL_CURSOR_TIMER > 60)
      GLOBAL_CURSOR_TIMER -= 60;
    if (GLOBAL_CURSOR_TIMER < 30) {
      draw_rectangle(out_commands, pos + offset + glm::vec2(text_size.x, 4),
                     {1, text_size.y - 8}, TEXTINPUT_COLOR_TEXT);
    }
  }
  draw_rectangle_outline(out_commands, pos, size, outline_color,
                         TEXTINPUT_ROUNDING, outline_size);
  draw_text(out_commands, pos + offset, m_text, text_color, m_font);
}

void TestTextInput::on_key(Key k) {
  if (k == Key::BACKSPACE) {
    if (m_text.size() > 0) {
      m_text.pop_back();
      auto size = get_size(*this);
      set_min_max_size(size, {10000, 10000});
    }
  }
}

void TestTextInput::on_text(char const *text) {
  m_text += text;
  auto size = get_size(*this);
  set_min_max_size(size, {10000, 10000});
}

void TestTextInput::lay(LayContext ctx) {
  ControlWidget::lay(ctx);
  auto wanted_size = get_size(*this);

  m_current_size = glm::max(wanted_size, ctx.min_size);

  m_pick_rect->m_current_size = m_current_size;
}

GPWidget *TestTextInput::get_hovered_gp(glm::vec2 pos) {
  return m_pick_rect->get_hovered_gp(pos);
}
