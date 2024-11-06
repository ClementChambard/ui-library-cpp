#ifndef BOXCONSTRAINTS_INCLUDED_H
#define BOXCONSTRAINTS_INCLUDED_H

#include "types.hpp"
#include <algorithm>
#include <cassert>

struct BoxConstraints {
    f32 min_width;
    f32 max_width;
    f32 min_height;
    f32 max_height;
    static BoxConstraints tight(Size size) { return { size.w, size.w, size.h, size.h }; }
    static BoxConstraints tight(f32 w, f32 h) { return { w, w, h, h }; }
    static BoxConstraints tight_w(f32 w) { return { w, w, 0.0, INFINITY }; }
    static BoxConstraints tight_h(f32 h) { return { 0.0, INFINITY, h, h }; }
    static BoxConstraints loose(Size size) { return { 0.0, size.w, 0.0, size.h }; }
    static BoxConstraints no_constraints() { return { 0.0, INFINITY, 0.0, INFINITY }; }
    BoxConstraints deflate(Insets edges) const {
        f32 h = edges.left + edges.right;
        f32 v = edges.top + edges.bottom;
        f32 deflated_min_width = std::max(0.f, min_width - h);
        f32 deflated_min_height = std::max(0.f, min_height - v);
        return { deflated_min_width, std::max(deflated_min_width, max_width - h), deflated_min_height, std::max(deflated_min_height, max_height - v) };
    }
    BoxConstraints loosen() const { return { 0.0, max_width, 0.0, max_height }; }
    BoxConstraints enforce(BoxConstraints const& other) const { return {
        std::clamp(min_width, other.min_width, other.max_width),
        std::clamp(max_width, other.min_width, other.max_width),
        std::clamp(min_height, other.min_height, other.max_height),
        std::clamp(max_height, other.min_height, other.max_height),
    }; }
    BoxConstraints tighten(f32 w, f32 h) const { return {
        std::clamp(w, min_width, max_width),
        std::clamp(w, min_width, max_width),
        std::clamp(h, min_height, max_height),
        std::clamp(h, min_height, max_height),
    }; }
    BoxConstraints tighten_w(f32 w) const { return {
        std::clamp(w, min_width, max_width),
        std::clamp(w, min_width, max_width),
        min_height,
        max_height,
    }; }
    BoxConstraints tighten_h(f32 h) const { return {
        min_width,
        max_width,
        std::clamp(h, min_height, max_height),
        std::clamp(h, min_height, max_height),
    }; }
    BoxConstraints flipped() const { return { min_height, max_height, min_width, max_width }; }
    BoxConstraints width_constraints() const { return { min_width, max_width, 0.0, INFINITY }; }
    BoxConstraints height_constraints() const { return { 0.0, INFINITY, min_height, max_height }; }
    f32 constrain_width(f32 width) const { return std::clamp(width, min_width, max_width); }
    f32 constrain_height(f32 height) const { return std::clamp(height, min_height, max_height); }
    Size constrain(Size size) const { return { constrain_width(size.w), constrain_height(size.h) }; }
    Size constrain(f32 w, f32 h) const { return { constrain_width(w), constrain_height(h) }; }
    Size constrain_size_and_attempt_to_preserve_aspect_ratio(Size size) const {
        if (is_tight()) {
            return smallest();
        }
        f32 width = size.w;
        f32 height = size.h;
        assert(width > 0.0);
        assert(height > 0.0);
        f32 aspect_ratio = width / height;
        if (width > max_width) {
            width = max_width;
            height = width / aspect_ratio;
        }
        if (height > max_height) {
            height = max_height;
            width = height * aspect_ratio;
        }
        if (width < min_width) {
            width = min_width;
            height = width / aspect_ratio;
        }
        if (height < min_height) {
            height = min_height;
            width = height * aspect_ratio;
        }
        return constrain(width, height);
    }
    Size biggest() const { return { max_width, max_height }; }
    Size smallest() const { return { min_width, min_height }; }
    bool has_tight_width() const { return max_width == min_width; }
    bool has_tight_height() const { return max_height == min_height; }
    bool is_tight() const { return has_tight_width() && has_tight_height(); }
    bool has_bounded_width() const { return max_width < INFINITY; }
    bool has_bounded_height() const { return max_height < INFINITY; }
    bool has_infinite_width() const { return max_width >= INFINITY; }
    bool has_infinite_height() const { return max_height >= INFINITY; }
    bool is_normalized() const { return min_width >= 0.0 && min_width <= max_width && min_height >= 0.0 && max_height >= min_height; }
    bool is_satisfied_by(Size size) const { return size.w >= min_width && size.w <= max_width && size.h >= min_height && size.h <= max_height; }
    BoxConstraints normalize() const {
        if (is_normalized()) {
            return *this;
        }
        f32 min_width = std::max(0.f, this->min_width);
        f32 min_height = std::max(0.f, this->min_height);
        return {
            min_width, std::max(this->min_width, max_width),
            min_height, std::max(this->min_height, max_height),
        };
    }
    BoxConstraints operator*(f32 factor) const { return { min_width * factor, max_width * factor, min_height * factor, max_height * factor }; }
    BoxConstraints operator/(f32 factor) const { return { min_width / factor, max_width / factor, min_height / factor, max_height / factor }; }
    BoxConstraints operator/(i32 factor) const { return { min_width / factor, max_width / factor, min_height / factor, max_height / factor }; }
    // pub fn lerp(a: &BoxConstraints, b: &BoxConstraints, t: f32) -> Self {
    //     if a == b { return a.clone(); }
    // return BoxConstraints(
    //   minWidth: a.minWidth.isFinite ? ui.lerpDouble(a.minWidth, b.minWidth, t)! : double.infinity,
    //   maxWidth: a.maxWidth.isFinite ? ui.lerpDouble(a.maxWidth, b.maxWidth, t)! : double.infinity,
    //   minHeight: a.minHeight.isFinite ? ui.lerpDouble(a.minHeight, b.minHeight, t)! : double.infinity,
    //   maxHeight: a.maxHeight.isFinite ? ui.lerpDouble(a.maxHeight, b.maxHeight, t)! : double.infinity,
    // );
    // }

    // pub fn lerp_to_unconstrained(a: &BoxConstraints, t: f32) -> Self {
    //     a.mul(1.0 - t)
    // }

    // pub fn lerp_from_unconstrained(a: &BoxConstraints, t: f32) -> Self {
    //     a.mul(t)
    // }

};

#endif // BOXCONSTRAINTS_INCLUDED_H
