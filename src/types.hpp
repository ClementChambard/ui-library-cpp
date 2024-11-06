#ifndef TYPES_HEADER_INCLUDED
#define TYPES_HEADER_INCLUDED

#include "defines.h"
#include <cmath>

namespace ns {

struct vec2 {
  union {
    f32 elements[2];
    struct {
      union {
        f32 x, r, s, u, w;
      };
      union {
        f32 y, g, t, v, h;
      };
    };
  };

  vec2() : x(0.0f), y(0.0f) {}
  vec2(vec2 const &other) : x(other.x), y(other.y) {}
  vec2(vec2 &&other) : x(other.x), y(other.y) {}
  vec2(f32 x, f32 y) : x(x), y(y) {}
  explicit vec2(f32 v) : x(v), y(v) {}

  vec2 &operator=(vec2 const &other) {
    x = other.x;
    y = other.y;
    return *this;
  }
  vec2 &operator=(vec2 &&other) {
    x = other.x;
    y = other.y;
    return *this;
  }
  vec2 operator+(vec2 const &other) const { return {x + other.x, y + other.y}; }
  vec2 operator+() const { return *this; }
  vec2 operator-(vec2 const &other) const { return {x - other.x, y - other.y}; }
  vec2 operator-() const { return {-x, -y}; }
  vec2 operator*(vec2 const &other) const { return {x * other.x, y * other.y}; }
  vec2 operator*(f32 f) const { return {x * f, y * f}; }
  vec2 operator/(vec2 const &other) const { return {x / other.x, y / other.y}; }
  vec2 operator/(f32 f) const { return {x / f, y / f}; }
  vec2 &operator+=(vec2 const &other) {
    x += other.x;
    y += other.y;
    return *this;
  }
  vec2 &operator-=(vec2 const &other) {
    x -= other.x;
    y -= other.y;
    return *this;
  }
  vec2 &operator*=(vec2 const &other) {
    x *= other.x;
    y *= other.y;
    return *this;
  }
  vec2 &operator/=(vec2 const &other) {
    x /= other.x;
    y /= other.y;
    return *this;
  }
  vec2 &operator*=(f32 f) {
    x *= f;
    y *= f;
    return *this;
  }
  vec2 &operator/=(f32 f) {
    x /= f;
    y /= f;
    return *this;
  }
  f32 operator[](usize i) const { return elements[i]; }
  f32 &operator[](usize i) { return elements[i]; }

  bool operator==(vec2 const &other) const {
    if (fabs(x - other.x) > 0.00001f)
      return false;
    if (fabs(y - other.y) > 0.00001f)
      return false;
    return true;
  }
  bool operator!=(vec2 const &other) const { return !(*this == other); }

  f32 length_sq() const { return x * x + y * y; }
  f32 length() const { return sqrt(length_sq()); }
  void normalize() {
    const f32 len_unsafe = length();
    const f32 len = len_unsafe == 0 ? 1 : len_unsafe;
    x /= len;
    y /= len;
  }
  vec2 normalized() const {
    vec2 o = *this;
    o.normalize();
    return o;
  }
  f32 dist_sq(vec2 const &other) const { return (*this - other).length_sq(); }
  f32 dist(vec2 const &other) const { return (*this - other).length(); }
  f32 dot(vec2 const &other) const { return x * other.x + y * other.y; }

  static vec2 zero() { return {0.0f, 0.0f}; }
  static vec2 one() { return {1.0f, 1.0f}; }
  static vec2 up() { return {0.0f, 1.0f}; }
  static vec2 down() { return {0.0f, -1.0f}; }
  static vec2 left() { return {-1.0f, 0.0f}; }
  static vec2 right() { return {1.0f, 0.0f}; }

};

} // namespace ns

using Size = ns::vec2;
using Position = ns::vec2;

struct Color {
    u8 r, g, b, a;
    Color(u8 r, u8 g, u8 b, u8 a) : r(r), g(g), b(b), a(a) {}
    Color(u32 hex) : r((hex >> 24) & 0xff), g((hex >> 16) & 0xff), b((hex >> 8) & 0xff), a((hex >> 0) & 0xff) {}
};

enum class Axis {
    Horizontal,
    Vertical
};

enum class TextDirection {
    LTR,
    RTL,
};

enum class VerticalDirection {
    Up,
    Down,
};

struct Insets {
    f32 top;
    f32 bottom;
    f32 left;
    f32 right;
};

#endif // TYPES_HEADER_INCLUDED
