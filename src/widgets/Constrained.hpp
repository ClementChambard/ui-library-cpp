#ifndef CONSTRAINED_H_
#define CONSTRAINED_H_

#include "../Widget.hpp"
#include "Flex.hpp"
#include <memory>

class ConstrainedBox : public ChildWidget {
    BoxConstraints constraints;
public:
    ConstrainedBox(BoxConstraints const& c, std::unique_ptr<Widget>&& child) : ChildWidget(std::move(child)), constraints(c) {}
    ConstrainedBox(BoxConstraints const& c, Widget* w = nullptr) : ConstrainedBox(c, std::unique_ptr<Widget>(w)) {}
    Size calculate_layout(BoxConstraints const& ctr) override {
        if (child) return child->calculate_layout(constraints.enforce(ctr));
        return constraints.enforce(ctr).constrain(Size{});
    }
};

class LimitedBox : public ChildWidget {
    Size max_size;
    BoxConstraints limit_constraints(BoxConstraints const& ctr);
    Size compute_size(BoxConstraints const& ctr);
public:
    LimitedBox(Size s, std::unique_ptr<Widget>&& child) : ChildWidget(std::move(child)), max_size(s) {}
    LimitedBox(f32 max_w, f32 max_h, std::unique_ptr<Widget>&& child) : ChildWidget(std::move(child)), max_size({max_w, max_h}) {}
    LimitedBox(Size s, Widget* w = nullptr) : LimitedBox(s, std::unique_ptr<Widget>(w)) {}
    LimitedBox(f32 max_w, f32 max_h, Widget* w = nullptr) : LimitedBox({max_w, max_h}, w) {}
    Size calculate_layout(BoxConstraints const& ctr) override {
        return compute_size(ctr);
    }
};

class SizedBox : public ConstrainedBox {
public:
    struct Expand {};
    struct Shrink {};
    SizedBox(Size s, std::unique_ptr<Widget>&& child) : ConstrainedBox(BoxConstraints::tight(s), std::move(child)) {}
    SizedBox(f32 w, f32 h, std::unique_ptr<Widget>&& child) : SizedBox({w, h}, std::move(child)) {}
    SizedBox(f32 wh, std::unique_ptr<Widget>&& child) : SizedBox({wh, wh}, std::move(child)) {}
    SizedBox(Expand, std::unique_ptr<Widget>&& child) : SizedBox({INFINITY, INFINITY}, std::move(child)) {}
    SizedBox(Shrink, std::unique_ptr<Widget>&& child) : SizedBox({0, 0}, std::move(child)) {}
    SizedBox(Size s, Widget* c = nullptr) : SizedBox(s, std::unique_ptr<Widget>(c)) {}
    SizedBox(f32 w, f32 h, Widget* c = nullptr) : SizedBox(Size{w, h}, c) {}
    SizedBox(f32 wh, Widget* c = nullptr) : SizedBox(Size{wh, wh}, c) {}
    SizedBox(Expand, Widget* c = nullptr) : SizedBox({INFINITY, INFINITY}, c) {}
    SizedBox(Shrink, Widget* c = nullptr) : SizedBox({0, 0}, c) {}
};

class Spacer : public Expanded {
public:
    Spacer() : Expanded(new SizedBox(SizedBox::Shrink{})) { flex(1); }
};

#endif // CONSTRAINED_H_
