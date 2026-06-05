
#include "mouse_area.hpp"
#include "../base/widget.hpp"

void ui::MouseArea::dispatch_event(ui::MouseEvent *ev) {
  ev->ma = this;
  ev->origin = parent;
  parent->dispatch_event(ev);
}
