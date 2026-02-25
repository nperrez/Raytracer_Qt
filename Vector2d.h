//
// Created by nicolas on 2/20/26.
//

#ifndef RAYTRACER_QT_VECTOR2D_H
#define RAYTRACER_QT_VECTOR2D_H
#include <cmath>


class Vector2d {

public:
    double x;
    double y;

    Vector2d(): x(0), y(0) {}
    Vector2d(const double x, const double y) {
        this->x = x;
        this->y = y;
    }

    Vector2d operator+(const Vector2d& v2) const {
        return Vector2d(x + v2.x, y + v2.y);
    }

    Vector2d operator-(const Vector2d& v2) const {
        return Vector2d(x - v2.x, y - v2.y);
    }

    Vector2d operator*(const double scalar) const {
        return Vector2d(x * scalar, y * scalar);
    }

    double length() const {
        return sqrt(x * x + y * y);
    }

};


#endif //RAYTRACER_QT_VECTOR2D_H