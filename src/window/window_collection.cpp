#include "window_collection.hpp"
#include <algorithm>

WindowCollection::WindowCollection() {
  add_event_listener(WINDOW_ACTIVATE_EVENT,
                     &WindowCollection::on_window_activate);
}

WindowCollection::~WindowCollection() {
  for (auto w : m_windows)
    delete w;
}

void WindowCollection::render_at(glm::vec2 pos, CmdList &out_commands) const {
  for (auto &w : m_windows)
    w->render_at(pos, out_commands);
}

void WindowCollection::lay(glm::vec2 min_size, glm::vec2 max_size) {
  m_current_size = max_size;
  for (auto &w : m_windows)
    w->lay(min_size, max_size);
}

void WindowCollection::on_window_activate(Event *e) {
  auto window = static_cast<Window *>(e->origin);
  if (window == m_windows.back())
    return;
  auto it = std::find(m_windows.begin(), m_windows.end(), window);
  assert(it != m_windows.end());
  m_windows.erase(it);
  m_windows.push_back(window);
}
