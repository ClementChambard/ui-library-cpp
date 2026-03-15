#include "window_manager.hpp"
#include "../control/control_widget.hpp"
#include "window.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keycode.h>
#include <algorithm>
#include <iostream>

WindowManager *WindowManager::INSTANCE = nullptr;

WindowManager::WindowManager() { INSTANCE = this; }

WindowManager::~WindowManager() {
  for (auto w : m_windows)
    delete w;
}

GPWidget *WindowManager::get_hovered_gp(glm::vec2 pos) {
  GPWidget *out = nullptr;
  for (auto w : m_windows) {
    if (!w->m_visible)
      continue;
    auto gp = w->get_hovered_gp(pos);
    if (gp)
      out = gp;
  }
  return out;
}

void WindowManager::register_window(Window *w) {
  m_windows.push_back(w);
  w->add_event_listener(WINDOW_ACTIVATE_EVENT, [this](Event *, Window *w) {
    this->activate_window(w);
  });
}

void WindowManager::activate_window(Window *w) {
  if (w == m_windows.back())
    return;
  // TODO: send deactivate event on currently activated window.
  unfocus();
  auto window = std::find(m_windows.begin(), m_windows.end(), w);
  if (window == m_windows.end())
    return;
  m_windows.erase(window);
  m_windows.push_back(w);

  m_gped.mouse_move(m_gped.mouse_pos, {0, 0});
}

void WindowManager::render_all(CmdList &cmds) {
  for (auto w : m_windows) {
    if (!w->m_visible)
      continue;
    w->render_at({}, cmds);
  }
}

static bool CURRENTLY_LISTENING_FOR_TEXT_INPUT = false;

bool is_allowed_key_repeat(SDL_Keycode k) {
  return k == SDLK_LEFT || k == SDLK_RIGHT || k == SDLK_DOWN || k == SDLK_UP ||
         k == SDLK_DELETE || k == SDLK_BACKSPACE;
}

bool is_event_key(SDL_Keycode k) {
  return is_allowed_key_repeat(k) || k == SDLK_ESCAPE || k == SDLK_RETURN;
}

Key translate_key(SDL_Keycode k) {
  switch (k) {
  case SDLK_LEFT:
    return Key::LEFT;
  case SDLK_RIGHT:
    return Key::RIGHT;
  case SDLK_DOWN:
    return Key::DOWN;
  case SDLK_UP:
    return Key::UP;
  case SDLK_DELETE:
    return Key::DELETE;
  case SDLK_BACKSPACE:
    return Key::BACKSPACE;
  case SDLK_ESCAPE:
    return Key::ESCAPE;
  case SDLK_RETURN:
    return Key::RETURN;
  default:
    return Key::UNKNOWN;
  }
}

void WindowManager::handle_event(SDL_Event const &e) {
  switch (e.type) {
  case SDL_EVENT_MOUSE_MOTION:
    m_gped.mouse_move({e.motion.x, e.motion.y}, {e.motion.xrel, e.motion.yrel});
    break;
  case SDL_EVENT_MOUSE_BUTTON_DOWN:
    m_gped.mouse_button_down(e.button.button, {e.button.x, e.button.y});
    break;
  case SDL_EVENT_MOUSE_BUTTON_UP:
    m_gped.mouse_button_up(e.button.button, {e.button.x, e.button.y});
    break;
  case SDL_EVENT_WINDOW_MOUSE_LEAVE:
    m_gped.mouse_leave();
    break;
  case SDL_EVENT_KEY_DOWN:
    if (e.key.repeat && !is_allowed_key_repeat(e.key.key))
      break;
    if (e.key.key == SDLK_TAB) {
      if ((e.key.mod & SDL_KMOD_SHIFT) != 0) {
        focus_prev();
      } else {
        focus_next();
      }
    }
    if (e.key.key == SDLK_ESCAPE) {
      unfocus();
    }
    if (is_event_key(e.key.key) && m_current_focus) {
      m_current_focus->on_key(translate_key(e.key.key));
    }
    break;
  case SDL_EVENT_TEXT_INPUT:
    if (m_current_focus)
      m_current_focus->on_text(e.text.text);
    break;
  }
}

void set_focus(SDL_Window *w, ControlWidget *&current, ControlWidget *to) {
  if (current == to)
    return;
  current = to;
  current->m_focused = true;
  if (current->m_need_text_input != CURRENTLY_LISTENING_FOR_TEXT_INPUT) {
    CURRENTLY_LISTENING_FOR_TEXT_INPUT = current->m_need_text_input;
    if (current->m_need_text_input) {
      SDL_StartTextInput(w);
    } else {
      SDL_StopTextInput(w);
    }
  }
}

void WindowManager::focus(struct ControlWidget *w) {
  if (m_current_focus != nullptr)
    m_current_focus->m_focused = false;
  if (w != nullptr)
    set_focus(m_main_window.w, m_current_focus, w);
  else
    m_current_focus = nullptr;
}

void focus_next_control(SDL_Window *sdlw, ControlWidget *&current,
                        ControlWidget *w,
                        ControlWidget *ControlWidget::*next_ptr) {
  if (current != nullptr) {
    current->m_focused = false;
    w = current->*next_ptr;
  }
  auto orig_w = w;
  while (w->m_disabled) {
    w = w->*next_ptr;
    if (w == orig_w)
      return;
  }
  set_focus(sdlw, current, w);
}

void WindowManager::focus_next() {
  auto list_next = m_windows.back()->m_control_list;
  if (list_next == nullptr)
    return;
  focus_next_control(m_main_window.w, m_current_focus, list_next,
                     &ControlWidget::m_next_control_in_window);
}

void WindowManager::focus_prev() {
  auto list_prev = m_windows.back()->m_control_list->m_prev_control_in_window;
  if (list_prev == nullptr)
    return;
  focus_next_control(m_main_window.w, m_current_focus, list_prev,
                     &ControlWidget::m_prev_control_in_window);
}

void WindowManager::unfocus() {
  if (m_current_focus == nullptr)
    return;
  m_current_focus->m_focused = false;
  m_current_focus = nullptr;
  CURRENTLY_LISTENING_FOR_TEXT_INPUT = false;
  SDL_StopTextInput(m_main_window.w);
}
