#ifndef RENDERCONTEXT_INCLUDED_H
#define RENDERCONTEXT_INCLUDED_H

#include "DrawBatch.hpp"
#include "types.hpp"
//#include <iostream>

struct RenderContext {
    Position pos = {0, 0};
    f32 z = 0.f;
    DrawBatch* b;
    void draw_rectangle(f32 x, f32 y, f32 w, f32 h, Color c, f32 z = 0.0) {
        (void) x, (void) y, (void) z, (void) w, (void) h, (void) c;
        // std::cout << "DRAW_RECT: " << x << "," << y << " - " << w << "x" << h << " z=" << z << "\n";
        b->draw_rectangle((i32)x, (i32)(x + w), (i32)y, (i32)(y + h), z, c);
    }
};

class PushRenderContextPosRAII {
    RenderContext* ctx_ptr;
    Position pos;
    float z;
public:
    PushRenderContextPosRAII(RenderContext& ctx) { pos = ctx.pos; z = ctx.z; ctx_ptr = &ctx; }
    ~PushRenderContextPosRAII() { ctx_ptr->pos = pos; ctx_ptr->z = z; }
};

#define push_rctx_pos(ctx) auto __push_render_context_pos_var__ = PushRenderContextPosRAII(ctx)

#endif // RENDERCONTEXT_INCLUDED_H
