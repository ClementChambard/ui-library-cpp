#include "Flex.hpp"
#include <cmath>

Size Flex::calculate_layout(BoxConstraints const& constraints) {
    if (!can_compute_intrinsics()) return {};
    auto [size, sizes] = compute_sizes(constraints);
    Size new_size = constraints.constrain(size);
    Size main_cross;
    switch (direction) {
        case Axis::Horizontal: main_cross = new_size; break;
        case Axis::Vertical: main_cross = {new_size.h, new_size.w}; break;
    }
    position_children(sizes, main_cross.w, main_cross.h);
    return new_size;
}

void Flex::render(RenderContext& context) {
    push_rctx_pos(context);
    context.pos += render_pos;
    for (auto& c : children) {
        c->render(context);
    }
}

void Flex::position_children(std::vector<Size> const& sizes, f32 main_size, f32 cross_size) {
    f32 free_space = 0.f;
    for (auto const& s : sizes) {
        switch (direction) {
            case Axis::Horizontal: free_space += s.w; break;
            case Axis::Vertical: free_space += s.h; break;
        }
    }
    auto [before, between, _] = distribute_free_space(main_axis_alignment, main_size - free_space, sizes.size());
    bool backwards = (direction == Axis::Horizontal && text_direction == TextDirection::RTL) || (direction == Axis::Vertical && vertical_direction == VerticalDirection::Up);
    bool cross_backwards = (direction == Axis::Vertical && text_direction == TextDirection::RTL) || (direction == Axis::Horizontal && vertical_direction == VerticalDirection::Up);
    f32 pos_here = before;
    for (i32 i = 0; auto &s : sizes) {
        auto m_c = direction == Axis::Horizontal ? s : Size{s.h, s.w};
        if (i != 0) pos_here += between;
        Size c_size = {pos_here, Flex::get_cross_offset(cross_axis_alignment, cross_size, m_c.h)};
        pos_here += m_c.w;
        if (cross_backwards) c_size.h = cross_size - c_size.h;
        if (backwards) c_size.w = main_size - c_size.w - m_c.w;
        children[i]->set_render_pos(direction == Axis::Horizontal ? c_size : Size{c_size.h, c_size.w});
        i++;
    }
}

std::tuple<Size, std::vector<Size>> Flex::compute_sizes(BoxConstraints const& constraints) {
    f32 total_flex = 0;
    f32 max_main_size = (direction == Axis::Horizontal) ? constraints.max_width : constraints.max_height;
    bool can_flex = max_main_size < INFINITY;
    f32 cross_size = 0.f;
    f32 allocated_size = 0.f;
    std::vector<Size> sizes;
    sizes.reserve(children.size());
    i32 last_flex_child_id = -1;
    for (i32 i = 0; auto &c : children) {
        i32 flex = c->props.get_i32("flex").value_or(0);
        if (flex > 0) {
            sizes.push_back({0, 0});
            total_flex += flex;
            last_flex_child_id = i;
        } else {
            BoxConstraints inner_constraints;
            if (cross_axis_alignment == CrossAxisStretch) {
                inner_constraints = (direction == Axis::Horizontal) ?
                    BoxConstraints::tight_h(constraints.max_height) :
                    BoxConstraints::tight_w(constraints.max_width);
            } else {
                inner_constraints = (direction == Axis::Horizontal) ?
                    BoxConstraints { 0.0, INFINITY, 0.0, constraints.max_height } :
                    BoxConstraints { 0.0, constraints.max_width, 0.0, INFINITY };
            }
            Size child_size = c->calculate_layout(inner_constraints);
            sizes.push_back(child_size);
            Size mc_size = (direction == Axis::Horizontal) ? child_size : Size{child_size.h, child_size.w};
            allocated_size += mc_size.w;
            cross_size = std::max(cross_size, mc_size.h);
        }
        i++;
    }
    f32 free_space = std::max(0.f, (can_flex ? max_main_size : 0.f) - allocated_size);
    f32 allocated_flex_space = 0.f;
    f32 space_per_flex = (can_flex && total_flex > 0) ? free_space / total_flex : NAN;
    for (i32 i = 0; auto &c : children) {
        i32 flex = c->props.get_i32("flex").value_or(0);
        if (flex > 0) {
            f32 max_child_extent = INFINITY;
            if (can_flex) {
                max_child_extent = (i == last_flex_child_id) ? free_space - allocated_flex_space : space_per_flex * flex;
            }
            f32 min_child_extent = (c->props.get_i32("fit").value_or(FitTight) == FitTight) ? max_child_extent : 0.f;
            BoxConstraints inner_constraints;
            if (cross_axis_alignment == CrossAxisStretch) {
                inner_constraints = (direction == Axis::Horizontal) ?
                    BoxConstraints{min_child_extent, max_child_extent, constraints.max_height, constraints.max_height} :
                    BoxConstraints{constraints.max_width, constraints.max_width, min_child_extent, max_child_extent};
            } else {
                inner_constraints = (direction == Axis::Horizontal) ?
                    BoxConstraints{min_child_extent, max_child_extent, 0.0, constraints.max_height } :
                    BoxConstraints{ 0.0, constraints.max_width, min_child_extent, max_child_extent };
            }
            Size child_size = c->calculate_layout(inner_constraints);
            sizes[i] = child_size;
            Size mc_size = (direction == Axis::Horizontal) ? child_size : Size{child_size.h, child_size.w};
            assert(mc_size.w <= max_child_extent);
            allocated_size += mc_size.w;
            allocated_flex_space += max_child_extent;
            cross_size = std::max(cross_size, mc_size.h);
        }
        i++;
    }
    f32 ideal_size = (can_flex && main_axis_size == MainAxisMax) ? max_main_size : allocated_size;
    Size out_size = (direction == Axis::Horizontal) ? Size{ideal_size, cross_size} : Size{cross_size, ideal_size};
    return std::tie(out_size, sizes);
}
