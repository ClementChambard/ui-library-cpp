#include "main_window.hpp"
#include <SDL3/SDL.h>
#include "../base/cursor.hpp"

void MainWindow::init(const char *name, glm::vec2 size) {
  wnd_size = size;
  SDL_Init(SDL_INIT_VIDEO);
  w = SDL_CreateWindow(name, wnd_size.x, wnd_size.y,
                        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
  ctx = SDL_GL_CreateContext(w);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetSwapInterval(1);
  Cursor::init();
}

void MainWindow::cleanup() {
  Cursor::cleanup();
  SDL_GL_DestroyContext((SDL_GLContext)ctx);
  SDL_DestroyWindow(w);
  SDL_Quit();
}

void MainWindow::swap() { SDL_GL_SwapWindow(w); }
