#include "window.hpp"
#include "../base/cursor.hpp"
#include "../control/control_widget.hpp"
#include "window_manager.hpp"

using namespace ui;

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

static glm::vec2 get_resize_handle_size(u32 i, glm::vec2 window_size) {
  auto const &d = WINDOW_RESIZE_HANDLE_DATA[i];
  f32 w = WINDOW_RESIZE_HANDLE_SIZE;
  if (d.w != 0)
    w = window_size.x - WINDOW_RESIZE_HANDLE_SIZE;
  f32 h = WINDOW_RESIZE_HANDLE_SIZE;
  if (d.h != 0)
    h = window_size.y - WINDOW_RESIZE_HANDLE_SIZE;
  return {w, h};
}

static glm::vec2 get_resize_handle_pos(u32 i, glm::vec2 window_size) {
  auto const &d = WINDOW_RESIZE_HANDLE_DATA[i];
  f32 x = 0;
  if (d.x == -1)
    x = -WINDOW_RESIZE_HANDLE_SIZE / 2.f;
  if (d.x == 1)
    x = window_size.x - WINDOW_RESIZE_HANDLE_SIZE / 2.f;
  f32 y = 0;
  if (d.y == -1)
    y = -WINDOW_RESIZE_HANDLE_SIZE / 2.f;
  if (d.y == 1)
    y = window_size.y - WINDOW_RESIZE_HANDLE_SIZE / 2.f;
  return {x, y};
}

static void set_resize_handle_cursor(u32 i) {
  Cursor::set(WINDOW_RESIZE_HANDLE_DATA[i].cursor);
}

static void on_resize_window(Window &w, glm::vec2 direction, u32 handle_id) {
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

static constexpr u32 NO_HANDLE = 0xffffffff;
static u32 get_resize_handle_id(MouseArea *ma, MARect *array) {
  static constexpr u32 handle_count =
      sizeof(Window::m_resize_handles) / sizeof(Window::m_resize_handles[0]);
  if (ma >= array && ma < array + handle_count) {
    return u32((MARect *)ma - array);
  } else {
    return NO_HANDLE;
  }
}

Window::Window() : Widget() {
  WindowManager::INSTANCE->register_window(this);

  add_event_listener<MouseDragEvent>(
      event::MOUSE_DRAG, this, [](MouseDragEvent *e, Window *w) {
        if (e->ma == &w->m_move_handle) {
          w->set_pos(w->m_pos + e->pos);
        } else if (u32 id = get_resize_handle_id(e->ma, w->m_resize_handles);
                   id != NO_HANDLE) {
          on_resize_window(*w, e->pos, id);
        }
      });

  add_event_listener(event::MOUSE_PRESS, [](Event *, Window *w) {
    Event e(WINDOW_ACTIVATE_EVENT);
    w->dispatch_event(&e);
  });

  add_event_listener<MouseEvent>(
      event::MOUSE_ENTER, this, [](MouseEvent *e, Window *w) {
        if (u32 id = get_resize_handle_id(e->ma, w->m_resize_handles);
            id != NO_HANDLE) {
          w->m_current_resize_handle = id;
          set_resize_handle_cursor(id);
        }
      });

  add_event_listener<MouseEvent>(
      event::MOUSE_LEAVE, this, [](MouseEvent *e, Window *w) {
        if (get_resize_handle_id(e->ma, w->m_resize_handles) != NO_HANDLE) {
          w->m_current_resize_handle = 0;
          Cursor::reset();
        }
      });
}

Window::~Window() { delete m_content; }

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

MouseArea *Window::get_hovered_ma(glm::vec2 pos, glm::vec2 *out_pos) {
  pos -= m_pos;

  MouseArea *ma = nullptr;

  if (!m_fit_to_content) {
    for (u32 i = 0; i < 8; i++) {
      ma = m_resize_handles[i].check(
          pos - get_resize_handle_pos(i, m_current_size), out_pos);
      if (ma != nullptr)
        return ma;
    }
  }

  if (m_content)
    ma = m_content->get_hovered_ma(pos - m_content_offset, out_pos);
  if (ma)
    return ma;

  ma = m_move_handle.check(pos, out_pos);
  if (ma)
    return ma;

  return m_event_fallback.check(pos, out_pos);
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

  m_event_fallback.set_size(m_current_size);
  m_move_handle.set_size(m_current_size);

  for (u32 i = 0; i < 8; i++) {
    m_resize_handles[i].set_size(get_resize_handle_size(i, m_current_size));
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
