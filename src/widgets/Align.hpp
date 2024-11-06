#ifndef ALIGN_H_
#define ALIGN_H_

#include "../Widget.hpp"
#include <memory>

class Align : public ChildWidget {
public:
    enum Alignment {
        TopLeft,
        TopMiddle,
        TopRight,
        CenterLeft,
        Center,
        CenterRight,
        BottomLeft,
        BottomMiddle,
        BottomRight,
        Value,
    };
private:
    Alignment alignment; // TODO: handle Value alignment
    Size factor = {1, 1};
    static Position get_align_pos(Alignment a) {
        switch (a) {
        case TopLeft: return {-1.0, -1.0};
        case TopMiddle: return {0, -1.0};
        case TopRight: return {1.0, -1.0};
        case CenterLeft: return {-1.0, 0};
        case Center: return {0, 0};
        case CenterRight: return {1.0, 0};
        case BottomLeft: return {-1.0, 1.0};
        case BottomMiddle: return {0, 1.0};
        case BottomRight: return {1.0, 1.0};
        default: return {0, 0};
        }
    }
public:
    Align(Alignment a, std::unique_ptr<Widget> &&child) : ChildWidget(std::move(child)), alignment(a) {}
    Align(Alignment a, Widget* child) : Align(a, std::unique_ptr<Widget>(child)) {}
    Align& with_width_factor(f32 wf) { factor.w = wf; return *this; }
    Align& with_height_factor(f32 hf) { factor.h = hf; return *this; }
    Align& with_factor(f32 wf, f32 hf) { factor = {wf, hf}; return *this; }
    Align& with_factor(Size f) { factor = f; return *this; }
    Size calculate_layout(BoxConstraints const& ctr) override {
        Position p = get_align_pos(alignment);
        Size wanted_size = child->calculate_layout(ctr.loosen());
        Size size = ctr.constrain(wanted_size * factor);
        p = (size - wanted_size) * (p + Position{1.0, 1.0}) / 2.0;
        child->set_render_pos(p);
        return size;
    }
};

class Center : public Align {
public:
    Center(std::unique_ptr<Widget> &&child) : Align(Align::Center, std::move(child)) {}
    Center(Widget* child) : Center(std::unique_ptr<Widget>(child)) {}
};

#endif // ALIGN_H_
