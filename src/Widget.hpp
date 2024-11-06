#ifndef WIDGET_INCLUDED_H
#define WIDGET_INCLUDED_H

#include "RenderContext.hpp"
#include "BoxConstraints.hpp"
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>


class WidgetProps {
    union WidgetPropsTy {
        i8  _i8[8];
        u8  _u8[8];
        i16 _i16[4];
        u16 _u16[4];
        i32 _i32[2];
        u32 _u32[2];
        i64 _i64;
        u64 _u64;
        f32 _f32[2];
        f64 _f64;
        const char* _cstr;
    };
    std::map<std::string, WidgetPropsTy> map;
    std::optional<WidgetPropsTy> get_prop(const char* key) const {
        if (auto it = map.find(key); it != map.end()) return it->second;
        return std::nullopt;
    }
    void set_prop(const char* key, WidgetPropsTy val) { map.insert(std::make_pair(key, val)); }
public:
    void set(const char* key, i8 value) { WidgetPropsTy v; v._i8[0] = value; set_prop(key, v); }
    std::optional<i8> get_i8(const char* key) const { if (auto t = get_prop(key)) return t->_i8[0]; return std::nullopt; }
    void set(const char* key, i16 value) { WidgetPropsTy v; v._i16[0] = value; set_prop(key, v); }
    std::optional<i16> get_i16(const char* key) const { if (auto t = get_prop(key)) return t->_i16[0]; return std::nullopt; }
    void set(const char* key, i32 value) { WidgetPropsTy v; v._i32[0] = value; set_prop(key, v); }
    std::optional<i32> get_i32(const char* key) const { if (auto t = get_prop(key)) return t->_i32[0]; return std::nullopt; }
    void set(const char* key, i64 value) { WidgetPropsTy v; v._i64 = value; set_prop(key, v); }
    std::optional<i64> get_i64(const char* key) const { if (auto t = get_prop(key)) return t->_i64; return std::nullopt; }
    void set(const char* key, u8 value) { WidgetPropsTy v; v._u8[0] = value; set_prop(key, v); }
    std::optional<u8> get_u8(const char* key) const { if (auto t = get_prop(key)) return t->_u8[0]; return std::nullopt; }
    void set(const char* key, u16 value) { WidgetPropsTy v; v._u16[0] = value; set_prop(key, v); }
    std::optional<u16> get_u16(const char* key) const { if (auto t = get_prop(key)) return t->_u16[0]; return std::nullopt; }
    void set(const char* key, u32 value) { WidgetPropsTy v; v._u32[0] = value; set_prop(key, v); }
    std::optional<u32> get_u32(const char* key) const { if (auto t = get_prop(key)) return t->_u32[0]; return std::nullopt; }
    void set(const char* key, u64 value) { WidgetPropsTy v; v._u64 = value; set_prop(key, v); }
    std::optional<u64> get_u64(const char* key) const { if (auto t = get_prop(key)) return t->_u64; return std::nullopt; }
    void set(const char* key, f32 value) { WidgetPropsTy v; v._f32[0] = value; set_prop(key, v); }
    std::optional<f32> get_f32(const char* key) const { if (auto t = get_prop(key)) return t->_f32[0]; return std::nullopt; }
    void set(const char* key, f64 value) { WidgetPropsTy v; v._f64 = value; set_prop(key, v); }
    std::optional<f64> get_f64(const char* key) const { if (auto t = get_prop(key)) return t->_f64; return std::nullopt; };
};

class Widget {
protected:
    Position render_pos;
    Size render_size;
public:
    WidgetProps props;
    virtual ~Widget() = default;
    virtual void render(RenderContext&) {}
    virtual Size calculate_layout(BoxConstraints const&) { return {}; }
    void set_render_pos(Position pos) { render_pos = pos; }
    Position get_render_pos() { return render_pos; }
    void set_render_size(Size size) { render_size = size; }
    Size get_render_size() { return render_size; }
};

class ChildWidget : public Widget {
protected:
    std::unique_ptr<Widget> child;
public:
    ChildWidget(std::unique_ptr<Widget> &&child) : child(std::move(child)) {}
    void render(RenderContext& ctx) override {
        if (!child) return;
        push_rctx_pos(ctx);
        ctx.pos += render_pos;
        child->render(ctx);
    }
    Size calculate_layout(BoxConstraints const& ctr) override {
        if (child) return child->calculate_layout(ctr);
        return ctr.smallest();
    }
};

class WidgetList : public Widget {
protected:
    std::vector<std::unique_ptr<Widget>> children;
public:
    WidgetList() {}
    WidgetList(std::unique_ptr<Widget> w) { children.push_back(std::move(w)); }
    WidgetList(Widget* w) : WidgetList(std::unique_ptr<Widget>(w)) {}
    template<typename... Ts>
    WidgetList(Widget* w, Ts... ws) : WidgetList(ws...) { children.insert(children.begin(), std::unique_ptr<Widget>(w)); }
    template<typename... Ts>
    WidgetList(std::unique_ptr<Widget> w, Ts... ws) : WidgetList(ws...) { children.insert(children.begin(), std::move(w)); }
    WidgetList(std::vector<std::unique_ptr<Widget>> c) : children(std::move(c)) {}
    WidgetList& add_child(Widget* c) { add_child(std::unique_ptr<Widget>(c)); return *this; }
    WidgetList& add_child(std::unique_ptr<Widget> &&c)  { children.push_back(std::move(c)); return *this; }
    void render(RenderContext& ctx) override {
        push_rctx_pos(ctx);
        ctx.pos += render_pos;
        for (auto &c : children) c->render(ctx);
    }
    Size calculate_layout(BoxConstraints const& ctr) override {
        for (auto &c : children) c->calculate_layout(ctr);
        return ctr.smallest();
    }
};

template<typename T, typename ...Ts>
inline T* wi(Ts... args) { return new T(args...); }

#endif // WIDGET_INCLUDED_H
