#pragma once

#include "widget.hpp"
#include <algorithm>
#include <type_traits>

struct CollectionChildHolder {
  Widget *widget = nullptr;
  glm::vec2 pos{};
};

template <typename T>
concept CollectionChildHolder_c = std::is_base_of_v<CollectionChildHolder, T>;

template <CollectionChildHolder_c Holder = CollectionChildHolder>
struct CollectionWidget : Widget {
  CollectionWidget(Widget *parent = nullptr) : Widget(parent) {}
  ~CollectionWidget() override {
    for (auto const &c : m_children)
      delete c.widget;
  }

  struct GPWidget *get_hovered_gp(glm::vec2 pos) override {
    if (!point_in_widget(pos))
      return nullptr;
    GPWidget *out = nullptr;
    for (u32 i = 0; i < m_children.size(); i++) {
      auto gp = m_children[i].widget->get_hovered_gp(pos - m_children[i].pos);
      if (gp)
        out = gp;
    }
    return out;
  }

  void remove_child(Widget *w) {
    auto it = std::find_if(m_children.begin(), m_children.end(),
                           [w](Holder const &h) { return h.widget == w; });
    if (it != m_children.end())
      m_children.erase(it);
  }

  void _append(Widget *w) override {
    m_children.push_back({w, {0, 0}});
    calc_min_max_size();
  }

  std::vector<Holder> m_children;
};
