#include "panel.hpp"
#include <glm/fwd.hpp>

using namespace ui;

static constexpr f32 PANEL_PADDING = 8.f;
static constexpr f32 PANEL_SIZE_TOPBAR = 32.f;
static constexpr f32 PANEL_SIZE_RESIZE_HANDLE = 16.f;
static constexpr f32 PANEL_SIZE_BUTTON = 24.f;
static constexpr f32 PANEL_SPACING_BUTTON = 32.f;

static glm::vec2 _y(f32 y) { return {0, y}; }
static glm::vec2 _xy(f32 v) { return {v, v}; }

static u32 get_panel_btn(MouseArea *ma, MARect *btns) {
  static constexpr u32 btn_cnt =
      sizeof(Panel::m_panel_buttons) / sizeof(Panel::m_panel_buttons[0]);
  if (ma >= btns && ma < btns + btn_cnt) {
    return ((MARect *)ma - btns) + 1;
  } else {
    return 0;
  }
}

Panel::Panel() : Window() {
  m_content_offset = _xy(PANEL_PADDING) + _y(PANEL_SIZE_TOPBAR);
  m_panel_buttons[0].set_size({PANEL_SIZE_BUTTON, PANEL_SIZE_BUTTON});

  add_event_listener<MouseEvent>(
      event::MOUSE_ENTER, this, [](MouseEvent *e, Panel *w) {
        if (u32 id = get_panel_btn(e->ma, w->m_panel_buttons); id != 0)
          w->m_current_panel_button = id;
      });
  add_event_listener<MouseEvent>(
      event::MOUSE_LEAVE, this, [](MouseEvent *e, Panel *w) {
        if (get_panel_btn(e->ma, w->m_panel_buttons) != 0)
          w->m_current_panel_button = 0;
      });
  add_event_listener(event::MOUSE_PRESS, this, [](Event *, Panel *w) {
    if (w->m_current_panel_button == 1) {
      w->m_collapsed = !w->m_collapsed;
      if (w->m_collapsed) {
        w->m_saved_height = w->m_current_size.y;
        w->m_current_size.y = PANEL_SIZE_TOPBAR;
      } else {
        w->m_current_size.y = w->m_saved_height;
      }
    }
  });
}

static void draw_collapse_arrow(CmdList &cmds, glm::vec2 pos, bool collapsed) {
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

  draw_triangle(cmds, p1, p2, p3, ColorScheme::current().panel.fg);
}

static void draw_resize_handles(CmdList &cmds, glm::vec2 pos, glm::vec2 size,
                                u32 idx_of_current_resize_handle) {
  auto col = ColorScheme::current().secondary.border.base;
  if (idx_of_current_resize_handle != 0) {
    col = ColorScheme::current().secondary.border.hover;
  }
  glm::vec2 p1, p2, p3;
  p1 = pos + size;
  p2 = p1 - glm::vec2(PANEL_SIZE_RESIZE_HANDLE, 0);
  p3 = p1 - glm::vec2(0, PANEL_SIZE_RESIZE_HANDLE);
  draw_triangle(cmds, p1, p2, p3, col);
}

static void draw_topbar(Panel const &p, CmdList &cmds, glm::vec2 pos) {
  draw_rectangle(cmds, pos, glm::vec2(p.m_current_size.x, PANEL_SIZE_TOPBAR),
                 ColorScheme::current().panel.topbar);

  if (p.m_current_panel_button != 0) {
    f32 x = pos.x + PANEL_SPACING_BUTTON * (p.m_current_panel_button - 1) +
            PANEL_SIZE_TOPBAR / 2.f;
    f32 y = pos.y + PANEL_SIZE_TOPBAR / 2.f;
    draw_circle(cmds, {x, y}, 12.f, ColorScheme::current().panel.btn_hover);
  }

  // draw symbols on panel buttons...

  draw_collapse_arrow(cmds, pos, p.m_collapsed);
}

void Panel::render_at(glm::vec2 pos, CmdList &out_commands) const {
  auto const &color_scheme = ColorScheme::current();
  pos += m_pos;

  // draw_box_shadow(out_commands, pos + glm::vec2(300, 300), m_current_size,
  //                 {0, 0, 0, 80}, 20, 1);

  draw_topbar(*this, out_commands, pos);

  if (m_title != "") {
    glm::vec2 t_pos = pos + glm::vec2(32, 0);
    draw_text(out_commands, t_pos, m_title, color_scheme.panel.fg);
  }

  if (m_collapsed)
    return;

  draw_rectangle(out_commands, pos + glm::vec2(0, PANEL_SIZE_TOPBAR),
                 m_current_size - glm::vec2(0, PANEL_SIZE_TOPBAR),
                 color_scheme.secondary.bg.base);

  draw_rectangle_outline(out_commands, pos, m_current_size,
                         color_scheme.secondary.border.base, 1);

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

MouseArea *Panel::get_hovered_ma(glm::vec2 pos) {
  f32 x = pos.x - m_pos.x - (PANEL_SIZE_TOPBAR - PANEL_SIZE_BUTTON) / 2.f;
  f32 y = pos.y - m_pos.y - (PANEL_SIZE_TOPBAR - PANEL_SIZE_BUTTON) / 2.f;
  for (auto &b : m_panel_buttons) {
    if (b.check(glm::vec2(x, y)))
      return &b;
    x -= PANEL_SPACING_BUTTON;
  }

  return Window::get_hovered_ma(pos);
}
