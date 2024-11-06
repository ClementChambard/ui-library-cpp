#ifndef APP_H_
#define APP_H_

#include "Widget.hpp"
#include <memory>

class App {
    void update_size(Size s);
    void render();
    void* app_state = nullptr;
public:
    App(const char* wnd_name, Size wnd_size, std::unique_ptr<Widget> &&root);
    App(const char* wnd_name, Size wnd_size, Widget *root) : App(wnd_name, wnd_size, std::unique_ptr<Widget>(root)) {}
    ~App();
    std::unique_ptr<Widget> root;
    Size wnd_size;
    void run();
};

#endif // APP_H_
