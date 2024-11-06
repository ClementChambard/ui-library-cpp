#include "Constrained.hpp"

Size LimitedBox::compute_size(BoxConstraints const& ctr) {
    auto cns = limit_constraints(ctr);
    if (child) return ctr.constrain(child->calculate_layout(cns));
    return cns.constrain(Size{});
}

BoxConstraints LimitedBox::limit_constraints(BoxConstraints const& ctr) {
    return {
        ctr.min_width,
        ctr.has_bounded_width() ? ctr.max_width : ctr.constrain_width(max_size.w),
        ctr.min_height,
        ctr.has_bounded_height() ? ctr.max_height : ctr.constrain_height(max_size.h)
    };
}
