#pragma once

struct Cursor {
  enum Kind {
    ARROW,
    TEXTINPUT,
    RESIZEALL,
    RESIZENS,
    RESIZEEW,
    RESIZENESW,
    RESIZENWSE,
    HAND,
    WAIT,
    PROGRESS,
    NOTALLOWED,
    COUNT,
    NONE,
  };

  static void init();
  static void cleanup();
  static void set(Kind k);
  static void reset() { set(ARROW); }
};
