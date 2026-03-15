#include "panel.hpp"
#include "../gp/gp_rect.hpp"
#include <glm/fwd.hpp>

static constexpr f32 PANEL_PADDING = 8.f;
static constexpr f32 PANEL_SIZE_TOPBAR = 32.f;
static constexpr f32 PANEL_SIZE_RESIZE_HANDLE = 16.f;
static constexpr f32 PANEL_SIZE_BUTTON = 24.f;
static constexpr f32 PANEL_SPACING_BUTTON = 32.f;

static constexpr Color PANEL_COLOR_OUTLINE = {192, 192, 192, 255};
static constexpr Color PANEL_COLOR_TOPBAR = {192, 192, 192, 255};
static constexpr Color PANEL_COLOR_RESIZE_HANDLE = {192, 192, 192, 255};
static constexpr Color PANEL_COLOR_RESIZE_HANDLE_HOVERED = {174, 174, 174, 255};
static constexpr Color PANEL_COLOR_ON_TOPBAR = {255, 255, 255, 255};
static constexpr Color PANEL_COLOR_BACKGROUND = {240, 240, 240, 224};
static constexpr Color PANEL_COLOR_COLLAPSE_BG_HOVERED = {174, 174, 174, 255};

Panel::~Panel() {
  for (auto b : m_panel_buttons)
    delete b;
}

RectGPWidget *new_topbar_button(Panel &p, u32 id,
                                std::function<void(Event *, Panel *)> onclick) {
  // clang-format off
  auto rect = RectGPWidget::make_event_handle(&p, {PANEL_SIZE_BUTTON, PANEL_SIZE_BUTTON});
  p.add_event_listener(MOUSE_ENTER_EVENT, rect, [id](Event *, Panel *w) { w->m_current_panel_button = id; });
  p.add_event_listener(MOUSE_LEAVE_EVENT, rect, [](Event *, Panel *w) { w->m_current_panel_button = 0; });
  p.add_event_listener(MOUSE_PRESS_EVENT, rect, onclick);
  // clang-format on
  return rect;
}

static glm::vec2 _y(f32 y) { return {0, y}; }
static glm::vec2 _xy(f32 v) { return {v, v}; }

Panel::Panel() : Window() {
  m_content_offset = _xy(PANEL_PADDING) + _y(PANEL_SIZE_TOPBAR);
  m_panel_buttons[0] = new_topbar_button(*this, 1, [](Event *, Panel *p) {
    p->m_collapsed = !p->m_collapsed;
    if (p->m_collapsed) {
      p->m_saved_height = p->m_current_size.y;
      p->m_current_size.y = PANEL_SIZE_TOPBAR;
    } else {
      p->m_current_size.y = p->m_saved_height;
    }
  });
}

void draw_collapse_arrow(CmdList &cmds, glm::vec2 pos, bool collapsed) {
  glm::vec2 p1, p2, p3;
  if (collapsed) {
    p1 = pos + glm::vec2(11, 10);
    p2 = p1 + glm::vec2(10, 6);
    p3 = p1 + glm::vec2(0, 12);
  } else {
    p1 = pos + glm::vec2(10, 11);
    p2 = p1 + glm::vec2(12, 0);
    p3 = p1 + glm::vec2(6, 10);
  }

  draw_triangle(cmds, p1, p2, p3, PANEL_COLOR_ON_TOPBAR);
}

void draw_resize_handles(CmdList &cmds, glm::vec2 pos, glm::vec2 size,
                         u32 idx_of_current_resize_handle) {
  auto col = PANEL_COLOR_RESIZE_HANDLE;
  if (idx_of_current_resize_handle != 0) {
    col = PANEL_COLOR_RESIZE_HANDLE_HOVERED;
  }
  glm::vec2 p1, p2, p3;
  p1 = pos + size;
  p2 = p1 - glm::vec2(PANEL_SIZE_RESIZE_HANDLE, 0);
  p3 = p1 - glm::vec2(0, PANEL_SIZE_RESIZE_HANDLE);
  draw_triangle(cmds, p1, p2, p3, col);
}

void draw_topbar(Panel const &p, CmdList &cmds, glm::vec2 pos) {
  draw_rectangle(cmds, pos, glm::vec2(p.m_current_size.x, PANEL_SIZE_TOPBAR),
                 PANEL_COLOR_TOPBAR);

  if (p.m_current_panel_button != 0) {
    f32 x = pos.x + PANEL_SPACING_BUTTON * (p.m_current_panel_button - 1) +
            PANEL_SIZE_TOPBAR / 2.f;
    f32 y = pos.y + PANEL_SIZE_TOPBAR / 2.f;
    draw_circle(cmds, {x, y}, 12.f, PANEL_COLOR_COLLAPSE_BG_HOVERED);
  }

  // draw symbols on panel buttons...

  draw_collapse_arrow(cmds, pos, p.m_collapsed);
}

void Panel::render_at(glm::vec2 pos, CmdList &out_commands) const {
  pos += m_pos;

  draw_topbar(*this, out_commands, pos);

  if (m_title != "") {
    glm::vec2 t_pos = pos + glm::vec2(32, 0);
    draw_text(out_commands, t_pos, m_title, PANEL_COLOR_ON_TOPBAR);
  }

  if (m_collapsed)
    return;

  draw_rectangle(out_commands, pos + glm::vec2(0, PANEL_SIZE_TOPBAR),
                 m_current_size - glm::vec2(0, PANEL_SIZE_TOPBAR),
                 PANEL_COLOR_BACKGROUND);

  draw_rectangle_outline(out_commands, pos, m_current_size, PANEL_COLOR_OUTLINE,
                         2);

  bool is_resizable = !m_fit_to_content;

  if (is_resizable) {
    draw_resize_handles(out_commands, pos, m_current_size,
                        m_current_resize_handle);
  }

  draw_set_scissor(out_commands, pos + _y(PANEL_SIZE_TOPBAR) + _xy(2),
                   m_current_size - _y(PANEL_SIZE_TOPBAR) - _xy(4));

  if (m_content)
    m_content->render_at(pos + m_content_offset, out_commands);

  draw_disable_scissor(out_commands);
}

void Panel::recalc_layout(glm::vec2) {
  if (m_fit_to_content) {
    m_current_size = m_minimum_size;
  } else {
    m_current_size = glm::max(m_current_size, m_minimum_size);
  }
  Window::recalc_layout(m_current_size - _xy(PANEL_PADDING * 2) -
                        _y(PANEL_SIZE_TOPBAR));
}

void Panel::calc_min_max_size() {
  auto size = _xy(PANEL_PADDING * 2) + _y(PANEL_SIZE_TOPBAR);
  if (m_content) {
    size = m_content->m_minimum_size + size;
  }
  set_min_max_size(size, {10000, 10000});
  recalc_layout({});
}

GPWidget *Panel::get_hovered_gp(glm::vec2 pos) {
  f32 x = pos.x - m_pos.x - (PANEL_SIZE_TOPBAR - PANEL_SIZE_BUTTON) / 2.f;
  f32 y = pos.y - m_pos.y - (PANEL_SIZE_TOPBAR - PANEL_SIZE_BUTTON) / 2.f;
  for (auto b : m_panel_buttons) {
    if (b->get_hovered_gp(glm::vec2(x, y)) == b)
      return b;
    x -= PANEL_SPACING_BUTTON;
  }

  return Window::get_hovered_gp(pos);
}
