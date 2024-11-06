#include "BoxConstraints.hpp"
#include "RenderContext.hpp"
#include "Widget.hpp"
#include <cmath>
#include <cstdio>
#include <memory>
#include "widgets/Align.hpp"
#include "widgets/Blob.hpp"
#include "widgets/Constrained.hpp"
#include "widgets/Flex.hpp"
#include "widgets/Position.hpp"
#include "App.hpp"

/*
 * <Widget "CustomWidget">
 *   <ConstrainedBox {150..Inf, 0..INFINITY}>
 *     <Row main_axis_size=min main_axis_alignment=space_between>
 *       <Blob 40x40 0xff0000ff />
 *       <ConstrainedBox tight_h(40)>
 *         <Column main_axis_alignment=space_around>
 *           <Blob 100x15 0xff0000ff />
 *           <Blob 90x15 0xff0000ff />
 *         </Column>
 *       </ConstrainedBox>
 *     </Row>
 *   </ConstrainedBox>
 * </Widget>
 *
 * <App 800x600>
 *   <Align alignment=bottom_right>
 *     <Column main_axis_size=min>
 *       <Expanded flex=2><Blob size={300., 20.}, color={255, 0, 0, 255} /></Expanded>
 *       <Expanded flex=1><Blob size={140., 30.}, color={0, 255, 0, 255} /></Expanded>
 *       <Blob 500x50 0x0000ffff />
 *       <Blob 400x100 0xffff00ff />
 *       <Blob 250x20 0x00ffffff />
 *       <Blob 200x50 0x000000ff />
 *       <Widget "CustomWidget" />
 *     </Column>
 *   </Align>
 *   <PositionBox 140 40 absolute><Elevate 10><Blob 200x200 0xff88ffff /></Elevate></PositionBox>
 * </App>
 */

class CustomWidget : public ConstrainedBox {
public:
    CustomWidget() : ConstrainedBox(BoxConstraints { 150, INFINITY, 0, INFINITY }, wi<Row>(
          wi<Blob>(40, 40, 0xff0000ff),
          wi<ConstrainedBox>(BoxConstraints::tight_h(40), wi<Column>(
              wi<Blob>(100, 15, 0xff0000ff),
              wi<Blob>(90, 15, 0xff0000ff)
          )->set_main_axis_alignment(Flex::MainAxisSpaceAround))
      )->set_main_axis_size(Flex::MainAxisMin)->set_main_axis_alignment(Flex::MainAxisSpaceBetween)) {}
};

int main() {
    App("Cpp UI Prototype", {800.f, 600.f}, wi<WidgetList>(
        wi<Align>(Align::BottomRight, wi<Column>(
             wi<Expanded>(wi<Blob>(300, 20, 0xff0000ff))->flex(2),
             wi<Expanded>(wi<Blob>(140, 30, 0x00ff00ff))->flex(1),
             wi<Blob>(500, 50, 0x0000ffff),
             wi<Blob>(400, 100, 0xffff00ff),
             wi<Blob>(250, 20, 0x00ffffff),
             //wi<Button>(200, 50)->on_press([](){printf("pressed\n");}),
             wi<Blob>(200, 50, 0x000000ff),
             wi<CustomWidget>()
        )->set_main_axis_size(Flex::MainAxisMin)),
        wi<PositionBox>(140, 40, wi<Elevate>(10, new Blob({200.0, 200.0}, 0xff88ffff)))->absolute()
    )).run();
    return 0;
}
