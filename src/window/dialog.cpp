#include "dialog.hpp"
#include "../control/standard_buttons.hpp"
#include "window_manager.hpp"
#include <glm/fwd.hpp>
#include <iostream>

using namespace ui;

static constexpr f32 DIALOG_PADDING = 11.f;
static constexpr f32 DIALOG_SIZE_TOPBAR = 32.f;
static constexpr f32 DIALOG_SIZE_BOTBAR = 60.f;

Dialog::~Dialog() { delete m_standard_buttons; }

static glm::vec2 _y(f32 y) { return {0, y}; }
static glm::vec2 _xy(f32 v) { return {v, v}; }

Dialog::Dialog() : Window() {
  m_content_offset = _xy(DIALOG_PADDING) + _y(DIALOG_SIZE_TOPBAR);
  m_standard_buttons = new StandardButtons(this);

  add_event_listener<StandardButtonClickEvent>(
      event::STANDARD_BUTTON_CLICK, m_standard_buttons,
      [](StandardButtonClickEvent *e, Dialog *d) {
        d->on_standard_button_click(e->std_id);
      });
}

void Dialog::set_modal(bool v) {
  if (m_is_modal == v)
    return;
  m_is_modal = v;
  if (m_is_modal) {
    WindowManager::INSTANCE->activate_window(this);
  }
}

void Dialog::set_standard_buttons(u32 v) { m_standard_buttons->set_mask(v); }

void Dialog::render_at(glm::vec2 pos, CmdList &out_commands) const {
  auto const &color_scheme = ColorScheme::current();
  auto const &colors = color_scheme.dialog;
  pos += m_pos;

  if (m_is_modal) {
    draw_rectangle(out_commands, {0, 0}, {10000, 10000}, {40, 40, 40, 180});
  }

  // drop shadow...

  draw_rectangle(out_commands, pos,
                 glm::vec2(m_current_size.x, DIALOG_SIZE_TOPBAR), colors.bg1, 6,
                 6, 0, 0);

  if (m_title != "") {
    glm::vec2 t_pos = pos + glm::vec2(8, 0);
    draw_text(out_commands, t_pos, m_title, colors.fg);
  }

  draw_rectangle(out_commands,
                 pos + glm::vec2(0, m_current_size.y - DIALOG_SIZE_BOTBAR + 1),
                 glm::vec2(m_current_size.x, DIALOG_SIZE_BOTBAR - 1),
                 color_scheme.secondary.bg.base, 0, 0, 6, 6);

  draw_rectangle(out_commands, pos + glm::vec2(0, DIALOG_SIZE_TOPBAR + 1),
                 m_current_size -
                     glm::vec2(0, DIALOG_SIZE_TOPBAR + DIALOG_SIZE_BOTBAR + 1),
                 m_is_modal ? colors.bg1 : colors.bg2);

  draw_rectangle_outline(out_commands, pos, m_current_size, colors.border, 6,
                         1);

  draw_rectangle(out_commands, pos + glm::vec2(0, DIALOG_SIZE_TOPBAR),
                 {m_current_size.x, 1}, colors.separator);
  draw_rectangle(out_commands,
                 pos + glm::vec2(0, m_current_size.y - DIALOG_SIZE_BOTBAR),
                 {m_current_size.x, 1}, colors.separator);

  m_standard_buttons->render_at(
      pos + glm::vec2(DIALOG_PADDING,
                      DIALOG_PADDING + m_current_size.y - DIALOG_SIZE_BOTBAR),
      out_commands);

  draw_set_scissor(out_commands, pos + _y(DIALOG_SIZE_TOPBAR) + _xy(2),
                   m_current_size - _y(DIALOG_SIZE_TOPBAR) - _xy(4));

  if (m_content)
    m_content->render_at(pos + m_content_offset, out_commands);

  draw_disable_scissor(out_commands);
}

void Dialog::recalc_layout(glm::vec2) {
  if (m_fit_to_content) {
    m_current_size = m_minimum_size;
  } else {
    m_current_size = glm::max(m_current_size, m_minimum_size);
  }
  Window::recalc_layout(m_current_size - _xy(DIALOG_PADDING * 2) -
                        _y(DIALOG_SIZE_TOPBAR + DIALOG_SIZE_BOTBAR));

  glm::vec2 sb_size = {m_current_size.x - DIALOG_PADDING * 2,
                       DIALOG_SIZE_BOTBAR - DIALOG_PADDING * 2};
  m_standard_buttons->lay({this, sb_size, sb_size});
}

void Dialog::calc_min_max_size() {
  auto size =
      _xy(DIALOG_PADDING * 2) + _y(DIALOG_SIZE_TOPBAR + DIALOG_SIZE_BOTBAR);
  if (m_content) {
    size = m_content->m_minimum_size + size;
  }
  size = glm::max(size, m_standard_buttons->m_minimum_size);
  set_min_max_size(size, {10000, 10000});
  recalc_layout({});
}

MouseArea *Dialog::get_hovered_ma(glm::vec2 pos, glm::vec2 *out_pos) {

  auto ma = m_standard_buttons->get_hovered_ma(
      pos - m_pos -
          glm::vec2(DIALOG_PADDING,
                    DIALOG_PADDING + m_current_size.y - DIALOG_SIZE_BOTBAR),
      out_pos);
  if (ma) {
    return ma;
  }
  ma = Window::get_hovered_ma(pos, out_pos);
  if (m_is_modal && ma == nullptr)
    ma = &m_event_fallback;
  return ma;
}

void Dialog::on_standard_button_click(u32 std_id) {
  std::cout << "standard button: " << std_id << '\n';
}
