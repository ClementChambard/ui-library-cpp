#include "control/button.hpp"
#include "gp/gp_circle.hpp"
#include "gp/gp_event_dispatcher.hpp"
#include "gp/gp_rect.hpp"
#include "layout/row_widget.hpp"
#include "render/frame_timing.hpp"
#include "render/main_window.hpp"
#include "render/renderer.hpp"
#include "window/panel.hpp"
#include "window/window_collection.hpp"
#include <SDL3/SDL_events.h>
#include <iostream>

void pump_system_events(Widget *root, bool &is_running, Renderer &ren,
                        GPEventDispatcher &gped, MainWindow &w) {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    switch (e.type) {
    case SDL_EVENT_QUIT:
      is_running = false;
      break;
    case SDL_EVENT_MOUSE_MOTION:
      gped.mouse_move(root, {e.motion.x, e.motion.y},
                      {e.motion.xrel, e.motion.yrel});
      break;
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
      gped.mouse_button_down(e.button.button, {e.button.x, e.button.y});
      break;
    case SDL_EVENT_MOUSE_BUTTON_UP:
      gped.mouse_button_up(e.button.button, {e.button.x, e.button.y});
      break;
    case SDL_EVENT_WINDOW_RESIZED:
      w.wnd_size = {e.window.data1, e.window.data2};
      ren.update_size(w.wnd_size);
      break;
    case SDL_EVENT_WINDOW_MOUSE_LEAVE:
      gped.mouse_leave();
      break;
    }
  }
}

Widget *make_vue() {
  WindowCollection *root = new WindowCollection;

  Window *panel_1 = new Panel(root);
  panel_1->set_pos({220.f, 200.f});
  panel_1->set_size({800.f, 600.f});

  Widget *row = new RowWidget(panel_1);
  panel_1->set_content(row);

  RectGPWidget *rect_1 = new RectGPWidget(row, {100, 100}, {0, 255, 255, 255});

  RectGPWidget *rect_2 = new RectGPWidget(row, {100, 150}, {255, 0, 0, 255});
  rect_2->m_radius = 20;

  CircleGPWidget *circle = new CircleGPWidget(row, 50, {255, 0, 255, 255});

  auto button = new MyWidget(row);
  row->add_event_listener<Event, Widget>(
      CLICK_EVENT, button, [](Event *, Widget *) { std::cout << "CLICK!\n"; });

  Window *panel_2 = new Panel(root);
  panel_2->set_pos({10.f, 100.f});
  panel_2->set_size({200.f, 200.f});

  Widget *row2 = new RowWidget(panel_2);
  panel_2->set_content(row2);

  rect_1 = new RectGPWidget(row2, {100, 100}, {0, 255, 255, 255});
  rect_1->outline(10);

  rect_2 = new RectGPWidget(row2, {100, 150}, {255, 0, 0, 255});
  rect_2->m_radius = 20;
  rect_2->outline(10);

  circle = new CircleGPWidget(row2, 50, {255, 0, 255, 255});
  circle->outline(10);

  return root;
}

int main() {
  auto root = make_vue();

  MainWindow main_window;
  Renderer r;
  main_window.init("test", {800, 600});
  r.init();
  r.update_size({800, 600});
  GPEventDispatcher gped;
  bool is_running = true;

  while (is_running) {
    frame_timer_start();

    pump_system_events(root, is_running, r, gped, main_window);

    // TODO: don't do it always
    root->lay(r.wnd_size, r.wnd_size);
    r.cmd_list.clear();
    root->render_at({}, r.cmd_list);
    r.render();

    main_window.swap();

    frame_timer_end();
  }

  r.cleanup();
  main_window.cleanup();
  delete root;

  return 0;
}
