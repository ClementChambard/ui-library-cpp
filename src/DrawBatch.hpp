#ifndef DRAWBACTH_H_
#define DRAWBACTH_H_

#include "types.hpp"

class DrawBatch {
    void* state;
public:
    DrawBatch();
    ~DrawBatch();
    void draw_rectangle(f32 x1, f32 x2, f32 y1, f32 y2, f32 z, Color c);
    void submit();
    void update_wnd_size(Size s);
};

#endif // DRAWBACTH_H_
