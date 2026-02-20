//
// Created by nicolas on 2/20/26.
//

#include "Vector2d.h"

#include <math.h>
#include <bits/valarray_after.h>

class Vector2D {

public:
    double x;
    double y;
    Vector2D(): x(0), y(0) {}
    Vector2D(const double x, const double y) : x(x), y(y) {}

    Vector2D add(const Vector2D v) const {
        return Vector2D(x + v.x, y + v.y);
    }

    Vector2D subtract(const Vector2D v) const {
        return Vector2D(x - v.x, y - v.y);
    }

    Vector2D multiply(const double s) const {
        return Vector2D(x * s, y * s);
    }

    double length() const {
        return sqrt(x * x + y * y);
    }

};
