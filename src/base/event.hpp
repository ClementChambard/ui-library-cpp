#pragma once

#include "../defines.hpp"
#include <functional>
#include <glm/glm.hpp>
#include <type_traits>

#include "event_kinds.hpp"

namespace ui {

struct Widget;

struct Event {
  event::Kind id = 0;
  Widget *origin = nullptr;
  bool consumed = false;

  Event(u32 id) : id(id) {}
};

template <typename T>
concept EventType = std::is_base_of_v<Event, T>;

template <typename T>
concept WidgetType = std::is_base_of_v<Widget, T>;

struct EventListenerBase {
  u32 id = 0;
  Widget *who = nullptr;
  bool handles_consumed_events = false;

  virtual ~EventListenerBase() = default;
  virtual void apply(Event *e, Widget *w) = 0;
};

template <EventType E, WidgetType W> struct EventListener : EventListenerBase {
  using F = std::function<void(E *, W *)>;

  EventListener(F fn, u32 id, Widget *who = nullptr) {
    this->fn = fn;
    this->id = id;
    this->who = who;
  }

  F fn;
  void apply(Event *e, Widget *w) override {
    return fn(static_cast<E *>(e), static_cast<W *>(w));
  }
};

template <EventType E, WidgetType W>
struct MethodEventListener : EventListenerBase {
  using F = void (W::*)(E *);

  MethodEventListener(F fn, u32 id, Widget *who = nullptr) {
    this->fn = fn;
    this->id = id;
    this->who = who;
  }

  F fn;
  void apply(Event *e, Widget *w) override {
    return (static_cast<W *>(w)->*fn)(static_cast<E *>(e));
  }
};

} // namespace ui
