#include "dialog.hpp"
#include "../control/standard_buttons.hpp"
#include "../gp/gp_rect.hpp"
#include "window_manager.hpp"
#include <glm/fwd.hpp>
#include <iostream>

static constexpr f32 DIALOG_PADDING = 11.f;
static constexpr f32 DIALOG_SIZE_TOPBAR = 32.f;
static constexpr f32 DIALOG_SIZE_BOTBAR = 60.f;

static constexpr Color DIALOG_COLOR_OUTLINE = {209, 209, 209, 255};
static constexpr Color DIALOG_COLOR_SEPARATOR = {226, 228, 227, 255};
static constexpr Color DIALOG_COLOR_TOPBAR = c_white;
static constexpr Color DIALOG_COLOR_BG = {244, 245, 246, 255};
static constexpr Color DIALOG_COLOR_BOTBAR = {241, 241, 241, 255};

static constexpr Color TEXT_COLOR = {61, 61, 61, 255};

Dialog::~Dialog() { delete m_standard_buttons; }

static glm::vec2 _y(f32 y) { return {0, y}; }
static glm::vec2 _xy(f32 v) { return {v, v}; }

Dialog::Dialog() : Window() {
  m_content_offset = _xy(DIALOG_PADDING) + _y(DIALOG_SIZE_TOPBAR);
  m_standard_buttons = new StandardButtons(this);

  add_event_listener<StandardButtonClickEvent>(
      STANDARD_BUTTON_CLICK_EVENT, m_standard_buttons,
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
  pos += m_pos;

  if (m_is_modal) {
    draw_rectangle(out_commands, {0, 0}, {10000, 10000}, {40, 40, 40, 180});
  }

  // drop shadow...

  draw_rectangle(out_commands, pos,
                 glm::vec2(m_current_size.x, DIALOG_SIZE_TOPBAR),
                 DIALOG_COLOR_TOPBAR, 6, 6, 0, 0);

  if (m_title != "") {
    glm::vec2 t_pos = pos + glm::vec2(8, 0);
    draw_text(out_commands, t_pos, m_title, TEXT_COLOR);
  }

  draw_rectangle(out_commands,
                 pos + glm::vec2(0, m_current_size.y - DIALOG_SIZE_BOTBAR + 1),
                 glm::vec2(m_current_size.x, DIALOG_SIZE_BOTBAR - 1),
                 DIALOG_COLOR_BOTBAR, 0, 0, 6, 6);

  draw_rectangle(out_commands, pos + glm::vec2(0, DIALOG_SIZE_TOPBAR + 1),
                 m_current_size -
                     glm::vec2(0, DIALOG_SIZE_TOPBAR + DIALOG_SIZE_BOTBAR + 1),
                 DIALOG_COLOR_BG);

  draw_rectangle_outline(out_commands, pos, m_current_size,
                         DIALOG_COLOR_OUTLINE, 6, 1);

  draw_rectangle(out_commands, pos + glm::vec2(0, DIALOG_SIZE_TOPBAR),
                 {m_current_size.x, 1}, DIALOG_COLOR_SEPARATOR);
  draw_rectangle(out_commands,
                 pos + glm::vec2(0, m_current_size.y - DIALOG_SIZE_BOTBAR),
                 {m_current_size.x, 1}, DIALOG_COLOR_SEPARATOR);

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

GPWidget *Dialog::get_hovered_gp(glm::vec2 pos) {

  auto gp = m_standard_buttons->get_hovered_gp(
      pos - m_pos -
      glm::vec2(DIALOG_PADDING,
                DIALOG_PADDING + m_current_size.y - DIALOG_SIZE_BOTBAR));
  if (gp)
    return gp;
  gp = Window::get_hovered_gp(pos);
  if (m_is_modal && gp == nullptr)
    gp = m_event_fallback;
  return gp;
}

void Dialog::on_standard_button_click(u32 std_id) {
  std::cout << "standard button: " << std_id << '\n';
}
