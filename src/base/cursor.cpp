#include "cursor.hpp"
#include "../defines.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_mouse.h>

static SDL_Cursor *s_MOUSE_CURSORS[Cursor::COUNT];
static Cursor::Kind s_CUR_CURSOR = Cursor::ARROW;
static u32 s_INIT_COUNT = 0;

void Cursor::cleanup() {
  s_INIT_COUNT--;
  if (s_INIT_COUNT > 0) return;
  for (u32 cursor_n = 0; cursor_n < Cursor::COUNT; cursor_n++)
    SDL_DestroyCursor(s_MOUSE_CURSORS[cursor_n]);
}

void Cursor::init() {
  s_INIT_COUNT++;
  if (s_INIT_COUNT > 1) return;
  s_MOUSE_CURSORS[ARROW] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT);
  s_MOUSE_CURSORS[TEXTINPUT] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_TEXT);
  s_MOUSE_CURSORS[RESIZEALL] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_MOVE);
  s_MOUSE_CURSORS[RESIZENS] =
      SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NS_RESIZE);
  s_MOUSE_CURSORS[RESIZEEW] =
      SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_EW_RESIZE);
  s_MOUSE_CURSORS[RESIZENESW] =
      SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NESW_RESIZE);
  s_MOUSE_CURSORS[RESIZENWSE] =
      SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NWSE_RESIZE);
  s_MOUSE_CURSORS[HAND] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_POINTER);
  s_MOUSE_CURSORS[WAIT] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAIT);
  s_MOUSE_CURSORS[PROGRESS] =
      SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_PROGRESS);
  s_MOUSE_CURSORS[NOTALLOWED] =
      SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NOT_ALLOWED);
}

void Cursor::set(Kind k) {
  if (s_CUR_CURSOR == k)
    return;
  if (k == NONE) {
    SDL_HideCursor();
  } else {
    if (s_CUR_CURSOR == NONE)
      SDL_ShowCursor();
    s_CUR_CURSOR = k;
    SDL_SetCursor(s_MOUSE_CURSORS[k]);
  }
}
