#include "App.hpp"
#include "BoxConstraints.hpp"
#include "DrawBatch.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_video.h>
#include <cstdlib>
#include <GL/glew.h>
#include <ctime>

struct AppState {
    SDL_Window *w;
    SDL_GLContext ctx;
    DrawBatch *b;
};

void App::update_size(Size s) {
    wnd_size = s;
    reinterpret_cast<AppState*>(app_state)->b->update_wnd_size(s);
    root->calculate_layout(BoxConstraints::tight(wnd_size));
    glViewport(0, 0, s.w, s.h);
}

void App::render() {
    glClearColor(1, 1, 1, 1);
    glClearDepth(1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    RenderContext context;
    context.b = reinterpret_cast<AppState*>(app_state)->b;
    root->render(context);
    context.b->submit();
}

App::App(const char* wnd_name, Size wnd_size, std::unique_ptr<Widget> &&root) : root(std::move(root)), wnd_size(wnd_size) {
    SDL_Init(SDL_INIT_VIDEO);
    AppState *state = new AppState;
    app_state = state;
    state->w = SDL_CreateWindow(wnd_name, 0, 0, wnd_size.w, wnd_size.h, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    state->ctx = SDL_GL_CreateContext(state->w);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetSwapInterval(1);
    glewInit();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_ADD);
    state->b = new DrawBatch;
    update_size(wnd_size);
}

App::~App() {
    AppState* s = reinterpret_cast<AppState*>(app_state);
    delete s->b;
    SDL_GL_DeleteContext(s->ctx);
    SDL_DestroyWindow(s->w);
    delete s;
    SDL_Quit();
}

void App::run() {
    SDL_Event e;
    bool is_running = true;
    const f64 fps = 60.0;
    const f64 target_frame_time = 1.0 / fps;
    while (is_running) {
        timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);
        f64 frame_start_time = now.tv_sec + now.tv_nsec * 0.000000001;

        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_QUIT:
                is_running = false;
                break;
            case SDL_WINDOWEVENT:
                switch (e.window.event) {
                    case SDL_WINDOWEVENT_RESIZED:
                        update_size(Size(e.window.data1, e.window.data2));
                        break;
                }
                break;
            }
        }
        render();
        AppState *state = reinterpret_cast<AppState*>(app_state);
        SDL_GL_SwapWindow(state->w);

        clock_gettime(CLOCK_MONOTONIC, &now);
        f64 frame_end_time = now.tv_sec + now.tv_nsec * 0.000000001;
        f64 frame_elapsed_time = frame_end_time - frame_start_time;
        f64 remaining_seconds = target_frame_time - frame_elapsed_time;
        if (remaining_seconds > 0.0) {
            timespec ts;
            ts.tv_sec = (int)remaining_seconds;
            ts.tv_nsec = (int)((remaining_seconds - ts.tv_sec - 0.001) * 1000000000);
            nanosleep(&ts, 0);
        }
    }
}
