#include "Vector2d.h"

#include <cmath>

//Constructors
Vector2d::Vector2d() {
    x = 0;
    y = 0;
}

Vector2d::Vector2d(const double x, const double y) {
    Vector2d::x = x;
    Vector2d::y = y;
}

//Getters
double Vector2d::getX() const {
    return x;
}

double Vector2d::getY() const {
    return y;
}

//Methods
Vector2d Vector2d::operator+(const Vector2d& v2) const {
    return Vector2d(x + v2.x, y + v2.y);
}

Vector2d Vector2d::operator-(const Vector2d& v2) const {
    return Vector2d(x - v2.x, y - v2.y);
}

Vector2d Vector2d::operator*(const double scalar) const {
    return Vector2d(x * scalar, y * scalar);
}

double Vector2d::length() const {
    return sqrt(x * x + y * y);
}