//
// Created by Nicolas Perrez on 26/02/2026.
//

#include "Sphere.h"

#include <cmath>

Sphere::Sphere(const Vector3d &center, const double radius, const Color &color) : center(center), radius(radius), color(color) {}

Vector3d Sphere::getCenter() const {
    return center;
}

double Sphere::getRadius() const {
    return radius;
}

Color Sphere::getColor() const {
    return color;
}

double Sphere::getDepth() const {
    return center.getZ();
}

double Sphere::intersect(Ray ray) const {
    const double a = ray.getDirection()*ray.getDirection();
    const Vector3d v = ray.getLocation() - center;
    const double b = 2*(ray.getDirection()*v);
    const double c = v*v - radius*radius;
    const double d = b*b - 4*a*c;
    if (d < 0) {
        return -1;
    }

    double lambda1 = (-b-sqrt(d))/(2*a);

    if (lambda1>=0) {
        return lambda1;
    }
    return (-b+sqrt(d))/(2*a);
}
