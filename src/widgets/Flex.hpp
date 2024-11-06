#ifndef FLEX_INCLUDED_H
#define FLEX_INCLUDED_H

#include "../Widget.hpp"
#include <memory>
#include <tuple>
#include <vector>

class Flex : public Widget {
public:
    enum FlexFit {
        FitTight = 0,
        FitLoose = 1,
    };
    enum MainAxisSize {
        MainAxisMin,
        MainAxisMax,
    };
    enum MainAxisAlignment {
        MainAxisStart,
        MainAxisEnd,
        MainAxisCenter,
        MainAxisSpaceBetween,
        MainAxisSpaceAround,
        MainAxisSpaceEvenly,
    };
    enum CrossAxisAlignment {
        CrossAxisStart,
        CrossAxisEnd,
        CrossAxisCenter,
        CrossAxisStretch,
        CrossAxisBaseline,
    };
    static f32 get_cross_offset(CrossAxisAlignment caa, f32 s_max, f32 s) {
        switch (caa) {
            case CrossAxisStretch: case CrossAxisStart: return 0.0; break;
            case CrossAxisEnd: return s_max - s; break;
            case CrossAxisCenter: return (s_max - s) / 2.0; break;
            case CrossAxisBaseline: return -1.0; // unimplemented
        }
        return -1.0;
    }
    static auto distribute_free_space(MainAxisAlignment maa, f32 free, usize n) {
        struct { f32 _1, _2, _3; } ret;
        switch (maa) {
            case MainAxisStart: ret._1 = ret._2 = 0.0, ret._3 = free; break;
            case MainAxisEnd: ret._1 = free, ret._2 = ret._3 = 0.0; break;
            case MainAxisCenter: ret._1 = ret._3 = free / 2.0, ret._2 = 0.0; break;
            case MainAxisSpaceBetween: ret._1 = ret._3 = 0.0, ret._2 = free / (n - 1.0); break;
            case MainAxisSpaceAround: ret._1 = ret._3 = free / (2.0 * n), ret._2 = free / f32(n); break;
            case MainAxisSpaceEvenly: ret._1 = ret._2 = ret._3 = free / (n + 1.0); break;
        }
        return ret;
    }

    void render(RenderContext& context) override;
    Size calculate_layout(BoxConstraints const& constraints) override;
    static std::unique_ptr<Flex> make();
    Flex* set_direction(Axis a) { direction = a; return this; }
    Flex* set_main_axis_alignment(MainAxisAlignment maa) { main_axis_alignment = maa; return this; }
    Flex* set_main_axis_size(MainAxisSize mas) { main_axis_size = mas; return this; }
    Flex* set_cross_axis_alignment(CrossAxisAlignment caa) { cross_axis_alignment = caa; return this; }
    Flex* set_text_direction(TextDirection td) { text_direction = td; return this; }
    Flex* set_vertical_direction(VerticalDirection vd) { vertical_direction = vd; return this; }
    Flex* add_child(std::unique_ptr<Widget>&& c) { children.push_back(std::move(c)); return this; }
    Flex* add_child(Widget* c) { children.push_back(std::unique_ptr<Widget>(c)); return this; }
private:
    Axis direction;
    MainAxisAlignment main_axis_alignment = MainAxisStart;
    MainAxisSize main_axis_size = MainAxisMax;
    CrossAxisAlignment cross_axis_alignment = CrossAxisCenter;
    TextDirection text_direction = TextDirection::LTR;
    VerticalDirection vertical_direction = VerticalDirection::Down;
    // text_baseline: Option<()>,
    // clip_behavior: Option<()>,
    std::vector<std::unique_ptr<Widget>> children;
    bool can_compute_intrinsics() const { return cross_axis_alignment != CrossAxisBaseline; }
    void position_children(std::vector<Size> const& sizes, f32 main_size, f32 cross_size);
    std::tuple<Size, std::vector<Size>> compute_sizes(BoxConstraints const& constraints);
    friend class Column;
    friend class Row;
public:
    Flex(Axis axis) : direction(axis) {}
    Flex(Axis axis, std::unique_ptr<Widget> w) : direction(axis) { children.push_back(std::move(w)); }
    Flex(Axis axis, Widget* w) : Flex(axis, std::unique_ptr<Widget>(w)) {}
    template<typename... Ts>
    Flex(Axis axis, Widget* w, Ts... ws) : Flex(axis, ws...) { children.insert(children.begin(), std::unique_ptr<Widget>(w)); }
    template<typename... Ts>
    Flex(Axis axis, std::unique_ptr<Widget> w, Ts... ws) : Flex(axis, ws...) { children.insert(children.begin(), std::move(w)); }
};

class Column : public Flex {
public:
    Column() : Flex(Axis::Vertical) {}
    template<typename... Ts> Column(Ts... args) : Flex(Axis::Vertical, args...) {}
};
class Row : public Flex {
public:
    Row() : Flex(Axis::Horizontal) {};
    template<typename... Ts> Row(Ts... args) : Flex(Axis::Horizontal, args...) {}
};

class Flexible : public ChildWidget {
public:
    Flexible(std::unique_ptr<Widget> &&child, Flex::FlexFit fit = Flex::FitLoose) : ChildWidget(std::move(child)) { props.set("fit", i32(fit)); }
    Flexible(Widget* child, Flex::FlexFit fit = Flex::FitLoose) : Flexible(std::unique_ptr<Widget>(child), fit) {}
    Flexible* flex(i32 f) { props.set("flex", f); return this; }
};

class Expanded : public Flexible {
public:
    Expanded(Widget* child) : Flexible(child, Flex::FitTight) {}
    Expanded(std::unique_ptr<Widget> &&child) : Flexible(std::move(child), Flex::FitTight) {}
};

#endif // FLEX_INCLUDED_H
