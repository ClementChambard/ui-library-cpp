#pragma once

#include "../defines.hpp"
#include <functional>
#include <glm/glm.hpp>
#include <type_traits>

static constexpr u32 MOUSE_ENTER_EVENT = 0;
static constexpr u32 MOUSE_LEAVE_EVENT = 1;
static constexpr u32 MOUSE_PRESS_EVENT = 2;
static constexpr u32 MOUSE_RELEASE_EVENT = 3;
static constexpr u32 MOUSE_DRAG_START_EVENT = 4;
static constexpr u32 MOUSE_DRAG_END_EVENT = 5;
static constexpr u32 MOUSE_DRAG_EVENT = 6;

static constexpr u32 MB_LEFT = 1;

struct Widget;

struct Event {
  u32 id = 0;
  Widget *origin = nullptr;
  bool consumed = false;

  Event(u32 id) : id(id) {}
};

struct MouseButtonEvent : Event {
  u32 button_id = 0;
  glm::vec2 pos{};

  MouseButtonEvent(bool press, u32 button_id, glm::vec2 pos)
      : Event(press ? MOUSE_PRESS_EVENT : MOUSE_RELEASE_EVENT),
        button_id(button_id), pos(pos) {}
};

struct MouseDragEvent : Event {
  glm::vec2 pos;

  MouseDragEvent(glm::vec2 pos) : Event(MOUSE_DRAG_EVENT), pos(pos) {}
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
