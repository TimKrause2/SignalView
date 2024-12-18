#ifndef ESFONTTYPES_H
#define ESFONTTYPES_H

#include <vector>

#ifdef _MSC_VER
#define MIN __min
#define MAX __max
#else
#define MIN std::min
#define MAX std::max
#endif


// Define some fixed size types.

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;


// A simple 32-bit pixel.

union Pixel32
{
  Pixel32()
  : integer(0) { }
  Pixel32(uint8 ri, uint8 gi, uint8 bi, uint8 ai = 255)
  {
    b = bi;
    g = gi;
    r = ri;
    a = ai;
  }

  uint32 integer;

  struct
  {
    uint8 r, g, b, a;
  };
};


struct Vec2
{
  Vec2() { }
  Vec2(float a, float b)
  : x(a), y(b) { }

  float x, y;
};


struct Rect
{
  Rect() { }
  Rect(float left, float top, float right, float bottom)
  : xmin(left), xmax(right), ymin(top), ymax(bottom) { }

  void Include(const Vec2 &r)
  {
    xmin = MIN(xmin, r.x);
    ymin = MIN(ymin, r.y);
    xmax = MAX(xmax, r.x);
    ymax = MAX(ymax, r.y);
  }

  float Width() const { return xmax - xmin + 1; }
  float Height() const { return ymax - ymin + 1; }

  float xmin, xmax, ymin, ymax;
};

// A horizontal pixel span generated by the FreeType renderer.

struct Span
{
  Span() { }
  Span(int _x, int _y, int _width, int _coverage)
  : x(_x), y(_y), width(_width), coverage(_coverage) { }

  int x, y, width, coverage;
};

typedef std::vector<Span> Spans;

#endif
