#pragma once

#include "../defines.hpp"
#include "../render/render_command.hpp"
#include "event.hpp"
#include <glm/glm.hpp>
#include <vector>

struct Widget {
  Widget(Widget *parent = nullptr);
  virtual ~Widget();
  Widget(Widget const &) = delete;
  Widget &operator=(Widget const &) = delete;

  virtual void render_at(glm::vec2 pos, CmdList &out_commands) const;
  virtual void lay(glm::vec2 min_size, glm::vec2 max_size);
  virtual void _append(Widget *child) { (void)child; };
  virtual struct GPWidget *get_hovered_gp(glm::vec2 pos);

  void dispatch_event(Event *e);

  template <EventType E = Event, typename This>
  void add_event_listener(this This &self, u32 id,
                          EventListener<E, This>::F fn) {
    self.m_event_listeners.push_back(new EventListener<E, This>(fn, id));
  }
  template <EventType E = Event, typename This>
  void add_event_listener(this This &self, u32 id, Widget *who,
                          EventListener<E, This>::F fn) {
    self.m_event_listeners.push_back(new EventListener<E, This>(fn, id, who));
  }
  template <EventType E = Event, typename This>
  void add_event_listener(this This &self, u32 id,
                          MethodEventListener<E, This>::F fn) {
    self.m_event_listeners.push_back(new MethodEventListener<E, This>(fn, id));
  }
  template <EventType E = Event, typename This>
  void add_event_listener(this This &self, u32 id, Widget *who,
                          MethodEventListener<E, This>::F fn) {
    self.m_event_listeners.push_back(
        new MethodEventListener<E, This>(fn, id, who));
  }

  glm::vec2 m_current_size{};
  std::vector<EventListenerBase *> m_event_listeners{};
  Widget *m_parent = nullptr;
  Widget *m_internal_widget = nullptr;
};
