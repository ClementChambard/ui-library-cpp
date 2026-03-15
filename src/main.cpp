#include "control/button.hpp"
#include "control/standard_buttons.hpp"
#include "control/test_textinput.hpp"
#include "gp/gp_circle.hpp"
#include "gp/gp_rect.hpp"
#include "layout/flex_widget.hpp"
#include "layout/row_widget.hpp"
#include "render/frame_timing.hpp"
#include "render/main_window.hpp"
#include "render/renderer.hpp"
#include "window/dialog.hpp"
#include "window/panel.hpp"
#include "window/window_manager.hpp"
#include <SDL3/SDL_events.h>
#include <iostream>

void make_vue() {
  Panel *panel_1 = new Panel;
  panel_1->set_pos({220.f, 200.f});
  panel_1->set_size({800.f, 600.f});
  panel_1->m_title = "Test buttons";

  FlexWidget *row = new FlexWidget(panel_1);
  row->m_direction = Flex::VERTICAL;
  row->m_gap = 8.f;
  row->m_main_axis_alignment = Flex::SPACE_AROUND;
  row->m_cross_axis_alignment = Flex::STRETCH;
  panel_1->m_fit_to_content = true;
  panel_1->set_content(row);

  auto button = new Button(row);
  button->set_text("Press me!!");
  auto button2 = new Button(row);
  button2->set_text("Press me too!!");
  auto button3 = new Button(row);
  button3->set_text("Not me.");
  button3->set_disabled(true);
  auto input = new TestTextInput(row);
  (void)input;

  Dialog *panel_2 = new Dialog;
  panel_2->set_pos({500.f, 200.f});
  panel_2->set_size({200.f, 200.f});
  panel_2->m_title = "My Dialog title";
  panel_2->set_standard_buttons(3);
  panel_2->m_visible = false;

  row->add_event_listener<Event, Widget>(
      CLICK_EVENT, button, [button2](Event *, Widget *) {
        static bool a = false;
        std::cout << "CLICK!\n";
        a = !a;
        if (a) {
          button2->set_text("long long long long long long");
        } else {
          button2->set_text("Press me too!!");
        }
      });
  row->add_event_listener<Event, Widget>(CLICK_EVENT, button2,
                                         [panel_2](Event *, Widget *) {
                                           std::cout << "CLICK 2!\n";
                                           panel_2->m_visible = true;
                                           panel_2->set_modal(true);
                                         });
  row->add_event_listener<Event, Widget>(
      CLICK_EVENT, button3,
      [](Event *, Widget *) { std::cout << "should not click!\n"; });

  panel_2->add_event_listener(STANDARD_BUTTON_CLICK_EVENT,
                              [](Event *, Dialog *d) {
                                d->m_visible = false;
                                d->m_is_modal = false;
                              });

  Widget *row2 = new RowWidget(panel_2);
  panel_2->set_content(row2);

  auto rect_1 = new RectGPWidget(row2, {100, 100}, {0, 255, 255, 255});
  rect_1->outline(10);

  auto rect_2 = new RectGPWidget(row2, {100, 150}, {255, 0, 0, 255});
  rect_2->m_radius = 20;
  rect_2->outline(10);

  auto circle = new CircleGPWidget(row2, 50, {255, 0, 255, 255});
  circle->outline(10);
}

int main() {
  Font f = Font::load("res/default_font.fnt");
  Font::DEFAULT = &f;
  WindowManager wm;
  Renderer renderer;

  make_vue();

  wm.m_main_window.init("test", {800, 600});
  renderer.init();
  renderer.update_size({800, 600});
  bool is_running = true;

  while (is_running) {
    frame_timer_start();

    SDL_Event e;
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_EVENT_QUIT) {
        is_running = false;
        break;
      }
      if (e.type == SDL_EVENT_WINDOW_RESIZED) {
        wm.m_main_window.wnd_size = {e.window.data1, e.window.data2};
        renderer.update_size(wm.m_main_window.wnd_size);
      }
      if (e.type == SDL_EVENT_KEY_DOWN && e.key.key == SDLK_R) {
        f = Font::load("res/default_font.fnt");
      }
      wm.handle_event(e);
    }

    renderer.cmd_list.clear();
    wm.render_all(renderer.cmd_list);

    renderer.render();

    wm.m_main_window.swap();

    frame_timer_end();
  }

  renderer.cleanup();
  wm.m_main_window.cleanup();

  return 0;
}
