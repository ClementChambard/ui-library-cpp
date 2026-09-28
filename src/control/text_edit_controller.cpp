#include "text_edit_controller.hpp"
#include "../mouse/mouse_area.hpp"
#include <iostream>

using namespace ui;

const TextEditControllerSettings TextEditControllerSettings::Default{};

bool TextEditController::on_key(Key k, Widget *w) {
  if (k == Key::BACKSPACE) {
    if (m_text.size() > 0) {
      m_text.pop_back();
      if (w && m_settings->send_change) {
        Event e(event::TEXT_CHANGE);
        w->dispatch_event(&e);
      }
      return true;
    }
  }
  return false;
}

bool TextEditController::on_text(char const *text, Widget *w) {
  m_text += text;
  if (w && m_settings->send_change) {
    Event e(event::TEXT_CHANGE);
    w->dispatch_event(&e);
  }
  return true;
}

void TextEditController::render_text(glm::vec2 pos, CmdList &out_commands,
                                     Font *fnt, Color col) const {
  glm::vec2 text_size = fnt->calc_string_size(m_text) * 2.f;
  glm::vec2 offset = (m_rect - text_size) / 2.f;

  if (m_active) {
    static u32 GLOBAL_CURSOR_TIMER = 0;
    GLOBAL_CURSOR_TIMER++;
    if (GLOBAL_CURSOR_TIMER > 60)
      GLOBAL_CURSOR_TIMER -= 60;
    if (GLOBAL_CURSOR_TIMER < 30) {
      draw_rectangle(out_commands, pos + offset + glm::vec2(text_size.x, 4),
                     {1, text_size.y - 8}, col);
    }
  }
  draw_text(out_commands, pos + offset, m_text, col, fnt);

  if (m_selecting) {
    glm::vec2 p1 = glm::min(m_last_mouse_pos, m_mouse_end_sel_pos);
    glm::vec2 p2 = glm::max(m_last_mouse_pos, m_mouse_end_sel_pos);
    draw_rectangle(out_commands, pos + p1, p2 - p1, {128, 128, 255, 80});
  }
}

bool TextEditController::mouse_event(Event *e) {
  if (e->id == event::MOUSE_PRESS) {
    m_last_mouse_pos = ((MouseButtonEvent *)e)->rel_pos;
    clear_selection();
    return true;
  } else if (e->id == event::MOUSE_DRAG_START && m_settings->can_select) {
    selection_start();
    return true;
  } else if (e->id == event::MOUSE_DRAG_END && m_settings->can_select) {
    // What to do ?
    return true;
  } else if (e->id == event::MOUSE_DRAG && m_settings->can_select) {
    update_selection(((MouseDragEvent *)e)->pos);
    return true;
  }
  return false;
}

void TextEditController::update_selection(glm::vec2 mouse_delta) {
  m_mouse_end_sel_pos += mouse_delta;
}
