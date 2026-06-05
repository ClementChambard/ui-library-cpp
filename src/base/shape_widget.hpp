#pragma once

#include "widget.hpp"

namespace ui {

struct ShapeWidget : Widget {
  enum Kind {
    RECTANGLE,
    CIRCLE,
  } kind;
  union {
    struct {
      f32 m_radius = 0.f; // TODO: different radius per corner ??
      glm::vec2 m_size{};
    } rectangle;
    struct {
      f32 m_radius = 1.f;
    } circle;
  };
  bool m_outline = false;
  f32 m_outline_size = 1.f;
  Color m_color{};

  static ShapeWidget *Circle(Widget *parent, f32 radius, Color color) {
    auto shp = new ShapeWidget(parent, 2 * radius, 2 * radius);
    shp->kind = CIRCLE;
    shp->m_color = color;
    shp->circle = {
        .m_radius = radius,
    };
    shp->set_min_max_size(shp->m_current_size, shp->m_current_size);
    return shp;
  }

  static ShapeWidget *Rectangle(Widget *parent, glm::vec2 size, Color color) {
    auto shp = new ShapeWidget(parent, size.x, size.y);
    shp->kind = RECTANGLE;
    shp->m_color = color;
    shp->rectangle = {.m_radius = 0, .m_size = size};
    shp->set_min_max_size(shp->m_current_size, shp->m_current_size);
    return shp;
  }

  virtual void render_at(glm::vec2 pos, CmdList &out_commands) const override;
  virtual void lay(LayContext ctx) override;

  void outline(f32 outline_size = 1.f);

private:
  ShapeWidget(Widget *parent, f32 w, f32 h) : Widget(parent) {
    m_current_size = {w, h};
  }
};

} // namespace ui
