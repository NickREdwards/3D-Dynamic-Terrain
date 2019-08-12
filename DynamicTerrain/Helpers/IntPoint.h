#ifndef ENGINE_HELPERS_INTPOINT_H
#define ENGINE_HELPERS_INTPOINT_H

#include <Windows.h>

namespace Engine
{
namespace Helpers
{

struct IntPoint2 {
	int X;
	int Y;

	IntPoint2() : X(0), Y(0) {}
	IntPoint2(int x, int y) : X(x), Y(y) {}

	void operator=(const IntPoint2& b) { X = b.X; Y = b.Y; }
	void operator=(const POINT& b) { X = b.x; Y = b.y; }

	IntPoint2 operator-(const IntPoint2& b) { return IntPoint2(X - b.X, Y - b.Y); }

	static IntPoint2 Zero() { return IntPoint2(0, 0); }
};

}
}

#endif