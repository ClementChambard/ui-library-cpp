#include "standard_buttons.hpp"

static constexpr f32 STANDARD_BUTTONS_GAP = 10;
static constexpr f32 STANDARD_BUTTONS_MIN_WIDTH = 100;

StandardButtons::StandardButtons(Widget *parent) : Widget() {
  m_parent = parent;
}

StandardButtons::~StandardButtons() {
  for (auto b : m_standard_buttons) {
    delete b;
  }
}

void StandardButtons::render_at(glm::vec2 pos, CmdList &out_commands) const {
  pos = pos + glm::vec2(m_current_size.x, 0);
  for (u32 i = 0; i < BUTTON_COUNT; i++) {
    if (((m_standard_button_mask >> i) & 1) == 0)
      continue;
    auto b = m_standard_buttons[i];
    pos.x -= b->m_current_size.x;
    b->render_at(pos, out_commands);
    pos.x -= STANDARD_BUTTONS_GAP;
  }
}

void StandardButtons::lay(LayContext ctx) {
  for (auto b : m_standard_buttons) {
    if (b)
      b->lay(
          {ctx.current_window, {STANDARD_BUTTONS_MIN_WIDTH, 0}, ctx.max_size});
  }
  m_current_size = ctx.min_size;
}

GPWidget *StandardButtons::get_hovered_gp(glm::vec2 pos) {
  if (!point_in_widget(pos))
    return nullptr;
  pos = pos - glm::vec2(m_current_size.x, 0);
  for (u32 i = 0; i < BUTTON_COUNT; i++) {
    if (((m_standard_button_mask >> i) & 1) == 0)
      continue;
    auto b = m_standard_buttons[i];
    pos.x += b->m_current_size.x;
    auto gp = b->get_hovered_gp(pos);
    if (gp != nullptr)
      return gp;
    pos.x += STANDARD_BUTTONS_GAP;
  }
  return nullptr;
}

void StandardButtons::calc_min_max_size() {
  glm::vec2 size = {0, 0};
  u32 gap_count = 0;
  for (u32 i = 0; i < BUTTON_COUNT; i++) {
    if (((m_standard_button_mask >> i) & 1) == 0)
      continue;
    gap_count++;
    auto s = m_standard_buttons[i]->m_minimum_size;
    s.x = glm::max(s.x, STANDARD_BUTTONS_MIN_WIDTH);
    size += s;
  }
  size.x += gap_count * STANDARD_BUTTONS_GAP;
  set_min_max_size(size, {10000, 10000});
}

void StandardButtons::set_mask(u32 v) {
  if (m_standard_button_mask == v)
    return;
  m_standard_button_mask = v;
  create_buttons();
  calc_min_max_size();
}

Button *create_button(u32 i) {
  std::string text;
  if (i == 0)
    text = "Ok";
  if (i == 1)
    text = "Cancel";
  auto btn = new Button(nullptr);
  btn->set_text(text);
  return btn;
}

void StandardButtons::create_buttons() {
  for (u32 i = 0; i < BUTTON_COUNT; i++) {
    if (((m_standard_button_mask >> i) & 1) == 0)
      continue;
    if (m_standard_buttons[i] != nullptr)
      continue;
    m_standard_buttons[i] = create_button(i);
    m_standard_buttons[i]->m_parent = this;
    add_event_listener(CLICK_EVENT, m_standard_buttons[i],
                       [i](Event *, StandardButtons *b) {
                         StandardButtonClickEvent e(1 << i);
                         b->dispatch_event(&e);
                       });
  }
}
