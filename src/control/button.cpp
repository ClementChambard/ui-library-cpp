#include "button.hpp"
#include "../base/cursor.hpp"

using namespace ui;

static constexpr f32 BUTTON_ROUNDING = 4.f;
static constexpr f32 BUTTON_OUTLINE_WIDTH = 1;
static constexpr f32 BUTTON_PADDING_X = 14;
static constexpr f32 BUTTON_PADDING_y = 2;
static constexpr glm::vec2 BUTTON_MIN_SIZE = {32, 32};

glm::vec2 get_size(Button const &b) {
  glm::vec2 inner_size = b.m_font->calc_string_size(b.m_text) * 2.f;
  glm::vec2 wanted_size =
      inner_size + glm::vec2(BUTTON_PADDING_X + BUTTON_OUTLINE_WIDTH,
                             BUTTON_PADDING_y + BUTTON_OUTLINE_WIDTH) *
                       2.f;
  wanted_size = glm::max(BUTTON_MIN_SIZE, wanted_size);
  return wanted_size;
}

Button::Button(Widget *parent) : ControlWidget(parent) {
  m_pick_rect.set_size({200, 80});
  m_font = Font::DEFAULT;

  add_event_listener(event::MOUSE_ENTER, this, &Button::on_enter);
  add_event_listener(event::MOUSE_LEAVE, this, &Button::on_leave);
  add_event_listener<MouseButtonEvent>(event::MOUSE_PRESS, this,
                                       &Button::on_press);
  add_event_listener<MouseButtonEvent>(event::MOUSE_RELEASE, this,
                                       &Button::on_release);
  add_event_listener(event::MOUSE_DRAG_END, this, &Button::on_drag_end);

  set_min_max_size(get_size(*this), {10000, 10000});
}

void Button::on_drag_end(Event *) { m_pressing = false; }

void Button::on_enter(Event *) {
  m_hovering = true;
  if (!is_disabled())
    Cursor::set(Cursor::HAND);
  // use specific cursor for disabled ?
}

void Button::on_leave(Event *) {
  m_hovering = false;
  Cursor::reset();
}

void Button::on_press(MouseButtonEvent *e) {
  if (e->button_id != MB_LEFT || is_disabled())
    return;
  m_pressing = true;
}

void Button::on_release(MouseButtonEvent *e) {
  if (e->button_id != MB_LEFT || is_disabled())
    return;
  if (!m_pressing)
    return;
  m_pressing = false;
  auto ev = Event(event::BUTTON_CLICK);
  dispatch_event(&ev);
}

void Button::set_text(std::string const &t) {
  m_text = t;
  auto size = get_size(*this);
  set_min_max_size(size, {10000, 10000});
}

void Button::render_at(glm::vec2 pos, CmdList &out_commands) const {
  glm::vec2 size = m_current_size;

  auto const &col_set = m_kind == PRIMARY ? ColorScheme::current().primary
                                          : ColorScheme::current().secondary;
  Color bg_color = col_set.bg.base;
  Color outline_color = col_set.border.base;
  Color text_color = col_set.fg.base;
  if (is_disabled()) {
    bg_color.a = 128;
    outline_color.a = 128;
    text_color.a = 128;
  } else {
    if (m_hovering) {
      bg_color = col_set.bg.hover;
      outline_color = col_set.border.hover;
      text_color = col_set.fg.hover;
    }
    if (m_pressing) {
      bg_color = col_set.bg.press;
      outline_color = col_set.border.press;
      text_color = col_set.fg.press;
    }
  }
  draw_rectangle(out_commands, pos, size, bg_color, BUTTON_ROUNDING);

  f32 outline_size = BUTTON_OUTLINE_WIDTH;
  if (is_focused())
    outline_size += 1.f;
  draw_rectangle_outline(out_commands, pos, size, outline_color,
                         BUTTON_ROUNDING, outline_size);
  glm::vec2 text_size = m_font->calc_string_size(m_text) * 2.f;
  glm::vec2 offset = (size - text_size) / 2.f;
  draw_text(out_commands, pos + offset, m_text, text_color, m_font);
}

void Button::on_key(Key k) {
  if (k != Key::RETURN)
    return;
  auto ev = Event(event::BUTTON_CLICK);
  dispatch_event(&ev);
}

void Button::lay(LayContext ctx) {
  ControlWidget::lay(ctx);
  auto wanted_size = get_size(*this);

  m_current_size = glm::max(wanted_size, ctx.min_size);

  m_pick_rect.set_size(m_current_size);
}
