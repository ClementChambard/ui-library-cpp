#ifndef BLOB_INCLUDED_H
#define BLOB_INCLUDED_H

#include "../Widget.hpp"

class Blob : public Widget {
    Size size;
    Color color;
public:
    Blob(f32 w, f32 h, Color c) : size(w, h), color(c) {}
    Blob(Size s, Color c) : size(s), color(c) {}
    Size calculate_layout(BoxConstraints const& constraints) override {
        render_size = constraints.constrain(size);
        return render_size;
    }
    void render(RenderContext& context) override {
        Position pos = context.pos + render_pos;
        context.draw_rectangle(pos.x, pos.y, render_size.w, render_size.h, color, context.z);
    }
    Blob* set_size(Size s) { size = s; return this; }
    Blob* set_color(Color c) { color = c; return this; }
};

#endif
