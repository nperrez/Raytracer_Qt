//
// Created by Nicolas Perrez on 26/02/2026.
//

#include "Vector3d.h"

Vector3d::Vector3d() : x(0), y(0), z(0){}

Vector3d::Vector3d(double x, double y, double z) : x(x), y(y), z(z) {}

double Vector3d::getX() const {
    return x;
}

double Vector3d::getY() const {
    return y;
}

double Vector3d::getZ() const {
    return z;
}

Vector3d Vector3d::operator+(const Vector3d &v) const {
    return Vector3d(x + v.x, y + v.y, z + v.z);
}

Vector3d Vector3d::operator-(const Vector3d &v) const {
    return Vector3d(x - v.x, y - v.y, z - v.z);
}

double Vector3d::operator*(const Vector3d &v) const {
    return x * v.x + y * v.y + z * v.z;
}

Vector3d Vector3d::operator*(double scalar) const {
    return Vector3d(x*scalar, y*scalar, z*scalar);
}