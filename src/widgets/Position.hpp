#ifndef POSITION_H_
#define POSITION_H_

#include "../Widget.hpp"

#include <memory>
class PositionBox : public Widget {
private:
    std::unique_ptr<Widget> child;
    Position pos;
    bool _absolute = false;
public:
    PositionBox(Position p, std::unique_ptr<Widget> &&child) : child(std::move(child)), pos(p) {}
    PositionBox(f32 x, f32 y, std::unique_ptr<Widget> &&child) : PositionBox({x, y}, std::move(child)) {}
    PositionBox(Position p, Widget* child) : PositionBox(p, std::unique_ptr<Widget>(child)) {}
    PositionBox(f32 x, f32 y, Widget* child) : PositionBox({x, y}, child) {}
    PositionBox* absolute() { _absolute = true; return this; }
    Size calculate_layout(const BoxConstraints&) override {
        child->calculate_layout(BoxConstraints::no_constraints());
        child->set_render_pos(pos);
        return Size{};
    }
    void render(RenderContext &ctx) override {
        push_rctx_pos(ctx);
        auto this_pos = _absolute ? Position{} : render_pos;
        ctx.pos = this_pos;
        child->render(ctx);
    }
};

class Elevate : public ChildWidget {
private:
    f32 z;
public:
    Elevate(f32 z, std::unique_ptr<Widget> &&child) : ChildWidget(std::move(child)), z(z) {}
    Elevate(f32 z, Widget *child) : Elevate(z, std::unique_ptr<Widget>(child)) {}
    void render(RenderContext &ctx) override {
        push_rctx_pos(ctx);
        ctx.pos += render_pos;
        ctx.z += z;
        child->render(ctx);
    }
};

#endif // POSITION_H_
