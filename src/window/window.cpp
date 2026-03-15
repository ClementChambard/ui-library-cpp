#include "window.hpp"
#include "../base/cursor.hpp"
#include "../control/control_widget.hpp"
#include "../gp/gp_rect.hpp"
#include "window_manager.hpp"

static constexpr f32 WINDOW_RESIZE_HANDLE_SIZE = 16.f;

struct ResizeHandleData {
  i8 x, y;
  bool w, h;
  Cursor::Kind cursor;
  i8 horiz_dir, vert_dir;
};

ResizeHandleData WINDOW_RESIZE_HANDLE_DATA[8] = {
    {-1, -1, 0, 0, Cursor::RESIZENWSE, -1, -1},
    {0, -1, 1, 0, Cursor::RESIZENS, 0, -1},
    {1, -1, 0, 0, Cursor::RESIZENESW, 1, -1},
    {-1, 0, 0, 1, Cursor::RESIZEEW, -1, 0},
    {1, 0, 0, 1, Cursor::RESIZEEW, 1, 0},
    {-1, 1, 0, 0, Cursor::RESIZENESW, -1, 1},
    {0, 1, 1, 0, Cursor::RESIZENS, 0, 1},
    {1, 1, 0, 0, Cursor::RESIZENWSE, 1, 1},
};

glm::vec2 get_resize_handle_size(u32 i, glm::vec2 window_size) {
  auto const &d = WINDOW_RESIZE_HANDLE_DATA[i];
  f32 w = WINDOW_RESIZE_HANDLE_SIZE;
  if (d.w != 0)
    w = window_size.x;
  f32 h = WINDOW_RESIZE_HANDLE_SIZE;
  if (d.h != 0)
    h = window_size.y;
  return {w, h};
}

glm::vec2 get_resize_handle_pos(u32 i, glm::vec2 window_size) {
  auto const &d = WINDOW_RESIZE_HANDLE_DATA[i];
  f32 x = 0;
  if (d.x == -1)
    x = -WINDOW_RESIZE_HANDLE_SIZE;
  if (d.x == 1)
    x = window_size.x;
  f32 y = 0;
  if (d.y == -1)
    y = -WINDOW_RESIZE_HANDLE_SIZE;
  if (d.y == 1)
    y = window_size.y;
  return {x, y};
}

void set_resize_handle_cursor(u32 i) {
  Cursor::set(WINDOW_RESIZE_HANDLE_DATA[i].cursor);
}

void on_resize_window(Window &w, glm::vec2 direction, u32 handle_id) {
  auto const &d = WINDOW_RESIZE_HANDLE_DATA[handle_id];
  auto new_size = w.m_current_size;

  if (d.horiz_dir == 0)
    direction.x = 0;
  if (d.vert_dir == 0)
    direction.y = 0;

  glm::vec2 new_pos = w.m_pos;
  if (d.horiz_dir == -1) {
    new_size.x -= direction.x;
    if (new_size.x >= w.m_minimum_size.x) {
      new_pos.x += direction.x;
    } else {
      new_pos.x += w.m_current_size.x - w.m_minimum_size.x;
    }
  } else {
    new_size.x += direction.x;
  }
  if (d.vert_dir == -1) {
    new_size.y -= direction.y;
    if (new_size.y >= w.m_minimum_size.y) {
      new_pos.y += direction.y;
    } else {
      new_pos.y += w.m_current_size.y - w.m_minimum_size.y;
    }
  } else {
    new_size.y += direction.y;
  }

  if (new_pos != w.m_pos)
    w.set_pos(new_pos);

  if (new_size != w.m_current_size)
    w.set_size(new_size);
}

RectGPWidget *new_resize_handle(Window &w, u32 id) {
  auto rect = RectGPWidget::make_event_handle(
      &w, get_resize_handle_size(id, w.m_current_size));
  w.add_event_listener(MOUSE_ENTER_EVENT, rect, [id](Event *, Window *w) {
    w->m_current_resize_handle = id;
    set_resize_handle_cursor(id);
  });
  w.add_event_listener(MOUSE_LEAVE_EVENT, rect, [](Event *, Window *w) {
    w->m_current_resize_handle = 0;
    Cursor::reset();
  });
  w.add_event_listener<MouseDragEvent>(
      MOUSE_DRAG_EVENT, rect,
      [id](MouseDragEvent *e, Window *w) { on_resize_window(*w, e->pos, id); });
  return rect;
}

Window::Window() : Widget() {
  WindowManager::INSTANCE->register_window(this);

  m_event_fallback = RectGPWidget::make_event_handle(this, m_current_size);

  // clang-format off
  m_move_handle = RectGPWidget::make_event_handle(this, m_current_size);
  add_event_listener<MouseDragEvent>(MOUSE_DRAG_EVENT, m_move_handle, [](MouseDragEvent *e, Window *w) { w->set_pos(w->m_pos + e->pos); });
  add_event_listener(MOUSE_PRESS_EVENT, [](Event *, Window *w) { Event e(WINDOW_ACTIVATE_EVENT); w->dispatch_event(&e); });
  // clang-format on

  for (u32 i = 0; i < 8; i++)
    m_resize_handles[i] = new_resize_handle(*this, i);
}

Window::~Window() {
  delete m_content;
  delete m_event_fallback;
  delete m_move_handle;
  for (auto h : m_resize_handles)
    delete h;
}

void Window::set_content(Widget *w) {
  m_content = w;
  calc_min_max_size();
}

void Window::render_at(glm::vec2 pos, CmdList &out_commands) const {
  pos += m_pos;
  draw_set_scissor(out_commands, pos, m_current_size);

  m_content->render_at(pos, out_commands);

  draw_disable_scissor(out_commands);
}

GPWidget *Window::get_hovered_gp(glm::vec2 pos) {
  pos -= m_pos;

  GPWidget *gp = nullptr;

  if (!m_fit_to_content) {
    for (u32 i = 0; i < 8; i++) {
      gp = m_resize_handles[i]->get_hovered_gp(
          pos - get_resize_handle_pos(i, m_current_size));
      if (gp != nullptr)
        return gp;
    }
  }

  if (m_content)
    gp = m_content->get_hovered_gp(pos - m_content_offset);
  if (gp)
    return gp;

  gp = m_move_handle->get_hovered_gp(pos);
  if (gp)
    return gp;

  return m_event_fallback->get_hovered_gp(pos);
}

void Window::calc_min_max_size() {
  // TODO: depends on resizable ...
  set_min_max_size(m_content->m_minimum_size, {10000, 10000});
  recalc_layout(m_current_size);
}

void Window::recalc_layout(glm::vec2 size) {
  m_control_list = nullptr;
  if (m_content) {
    m_content->lay({this, size, size});
  }

  m_event_fallback->m_current_size = m_current_size;
  m_move_handle->m_current_size = m_current_size;

  for (u32 i = 0; i < 8; i++) {
    m_resize_handles[i]->m_current_size =
        get_resize_handle_size(i, m_current_size);
  }
}

void Window::set_size(glm::vec2 size) {
  m_current_size = size;
  recalc_layout(size);
}

void Window::add_control(struct ControlWidget *w) {
  if (m_control_list == nullptr) {
    w->m_next_control_in_window = w;
    w->m_prev_control_in_window = w;
    m_control_list = w;
    return;
  }
  w->m_next_control_in_window = m_control_list;
  w->m_prev_control_in_window = m_control_list->m_prev_control_in_window;
  m_control_list->m_prev_control_in_window->m_next_control_in_window = w;
  m_control_list->m_prev_control_in_window = w;
}
