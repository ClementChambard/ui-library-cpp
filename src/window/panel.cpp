#include "panel.hpp"
#include "../gp/gp_circle.hpp"
#include "../gp/gp_polygon.hpp"
#include "../gp/gp_rect.hpp"

static constexpr f32 PANEL_PADDING = 8.f;
static constexpr f32 PANEL_TOPBAR_SIZE = 32.f;
static constexpr f32 PANEL_RESIZE_HANDLE_SIZE = 16.f;
static constexpr Color PANEL_OUTLINE_COLOR = {192, 192, 192, 255};
static constexpr Color PANEL_TOPBAR_COLOR = {192, 192, 192, 255};
static constexpr Color PANEL_RESIZE_HANDLE_COLOR = {192, 192, 192, 255};
static constexpr Color PANEL_RESIZE_HANDLE_COLOR_HOVERED = {174, 174, 174, 255};
static constexpr Color PANEL_BACKGROUND_COLOR = {240, 240, 240, 224};
static constexpr Color PANEL_COLLAPSE_ARROW_COLOR = {255, 255, 255, 255};
static constexpr Color PANEL_COLLAPSE_BG_COLOR_HOVERED = {174, 174, 174, 255};

void collapse_arrow_open(PolygonGPWidget *arr) {
  arr->m_points[1] = {12, 0};
  arr->m_points[2] = {6, 10};
}
void collapse_arrow_close(PolygonGPWidget *arr) {
  arr->m_points[1] = {10, 6};
  arr->m_points[2] = {0, 12};
}

void on_collapse_pressed(Panel *p) {
  if (p->m_collapsed) {
    collapse_arrow_open(p->m_collapse_arrow);
  } else {
    collapse_arrow_close(p->m_collapse_arrow);
  }
  p->m_collapsed = !p->m_collapsed;
}

Panel::~Panel() {
  delete m_topbar_rect;
  delete m_resize_handle;
  delete m_background_rect;
  delete m_outline_rect;
  delete m_collapse_circle_bg;
  delete m_collapse_arrow;
}

Panel::Panel(Widget *parent) : Window(parent) {
  // clang-format off
  m_topbar_rect = new RectGPWidget(this, {m_current_size.x, PANEL_TOPBAR_SIZE}, PANEL_TOPBAR_COLOR);

  m_resize_handle = new PolygonGPWidget(this, {
      {PANEL_RESIZE_HANDLE_SIZE, 0}, {PANEL_RESIZE_HANDLE_SIZE, PANEL_RESIZE_HANDLE_SIZE}, {0, PANEL_RESIZE_HANDLE_SIZE}
      }, PANEL_RESIZE_HANDLE_COLOR);
  add_event_listener(MOUSE_ENTER_EVENT, m_resize_handle, [](Event *, Panel *w) { w->m_resize_handle->m_color = PANEL_RESIZE_HANDLE_COLOR_HOVERED; });
  add_event_listener(MOUSE_LEAVE_EVENT, m_resize_handle, [](Event *, Panel *w) { w->m_resize_handle->m_color = PANEL_RESIZE_HANDLE_COLOR; });

  m_background_rect = new RectGPWidget(this, {m_current_size.x, m_current_size.y - PANEL_TOPBAR_SIZE}, PANEL_BACKGROUND_COLOR);
  add_event_listener<MouseDragEvent>(MOUSE_DRAG_EVENT, m_topbar_rect, [](MouseDragEvent *e, Panel *w) { w->set_pos(w->m_pos + e->pos); });
  add_event_listener<MouseDragEvent>(MOUSE_DRAG_EVENT, m_background_rect, [](MouseDragEvent *e, Panel *w) { w->set_pos(w->m_pos + e->pos); });
  add_event_listener<MouseDragEvent>(MOUSE_DRAG_EVENT, m_resize_handle, [](MouseDragEvent*e, Panel *w) { w->set_size(w->m_current_size + e->pos); });

  m_outline_rect = new RectGPWidget(this, {m_current_size.x, m_current_size.y}, PANEL_OUTLINE_COLOR);
  m_outline_rect->outline(2);

  m_min_size = {64.f, 64.f};

  m_collapse_arrow = new PolygonGPWidget(this, {{0, 0}, {12, 0}, {6, 10}}, PANEL_COLLAPSE_ARROW_COLOR);
  m_collapse_circle_bg = new CircleGPWidget(this, 12.f, c_transparent);
  add_event_listener(MOUSE_ENTER_EVENT, m_collapse_circle_bg, [](Event*, Panel *w) { w->m_collapse_circle_bg->m_color = PANEL_COLLAPSE_BG_COLOR_HOVERED; });
  add_event_listener(MOUSE_LEAVE_EVENT, m_collapse_circle_bg, [](Event*, Panel *w) { w->m_collapse_circle_bg->m_color = c_transparent; });
  add_event_listener(MOUSE_PRESS_EVENT, m_collapse_circle_bg, [](Event*, Panel *p) { on_collapse_pressed(p); });

  add_event_listener(MOUSE_PRESS_EVENT, [](Event *, Panel *w) { Event e(WINDOW_ACTIVATE_EVENT); w->dispatch_event(&e); });
  // clang-format on
}

static glm::vec2 _y(f32 y) { return {0, y}; }
static glm::vec2 _xy(f32 v) { return {v, v}; }

void Panel::render_at(glm::vec2 pos, CmdList &out_commands) const {
  pos += m_pos;

  m_topbar_rect->render_at(pos, out_commands);

  m_collapse_circle_bg->render_at(pos + _xy(4), out_commands);

  f32 ax = 10, ay = 10;
  if (m_collapsed)
    ax += 1;
  else
    ay += 1;
  m_collapse_arrow->render_at(pos + glm::vec2(ax, ay), out_commands);

  if (m_collapsed)
    return;

  m_background_rect->render_at(pos + _y(PANEL_TOPBAR_SIZE), out_commands);

  m_outline_rect->render_at(pos, out_commands);

  m_resize_handle->render_at(
      pos + m_current_size - _xy(PANEL_RESIZE_HANDLE_SIZE), out_commands);

  draw_set_scissor(out_commands, pos + _y(PANEL_TOPBAR_SIZE),
                   m_current_size - _y(PANEL_TOPBAR_SIZE));

  if (m_content)
    m_content->render_at(pos + _xy(PANEL_PADDING) + _y(PANEL_TOPBAR_SIZE),
                         out_commands);

  draw_disable_scissor(out_commands);
}

void Panel::recalc_layout() {
  m_topbar_rect->m_current_size.x = m_current_size.x;
  m_background_rect->m_current_size = m_current_size - _y(PANEL_TOPBAR_SIZE);
  m_outline_rect->m_current_size = m_current_size;

  glm::vec2 content_size =
      m_current_size - _xy(PANEL_PADDING * 2) - _y(PANEL_TOPBAR_SIZE);

  if (m_content)
    m_content->lay(content_size, content_size);
}

GPWidget *Panel::get_hovered_gp(glm::vec2 pos) {
  pos -= m_pos;

  auto gp = m_collapse_circle_bg->get_hovered_gp(pos - _xy(4));
  if (gp)
    return gp;

  gp = m_topbar_rect->get_hovered_gp(pos);
  if (gp)
    return gp;

  gp = m_resize_handle->get_hovered_gp(pos - m_current_size +
                                       _xy(PANEL_RESIZE_HANDLE_SIZE));
  if (gp)
    return gp;

  if (m_content)
    gp = m_content->get_hovered_gp(pos - _xy(PANEL_PADDING) -
                                   _y(PANEL_TOPBAR_SIZE));
  if (gp)
    return gp;

  return m_background_rect->get_hovered_gp(pos - _y(PANEL_TOPBAR_SIZE));
}
