//
// Created by Nicolas Perrez on 26/02/2026.
//

#include "Sphere.h"

#include <cmath>

Sphere::Sphere(const Vector3d &center, const double radius, const Material &material) : Object(material), center(center), radius(radius) {}

Vector3d Sphere::getCenter() const {
    return center;
}

double Sphere::getRadius() const {
    return radius;
}

double Sphere::getDepth() const {
    return center.getZ();
}

Hit Sphere::intersect(Ray ray) const {
    const double a = ray.getDirection()*ray.getDirection();
    const Vector3d v = ray.getLocation() - center;
    const double b = 2*(ray.getDirection()*v);
    const double c = v*v - radius*radius;
    const double d = b*b - 4*a*c;
    if (d < 0) {
        return Hit(-1, Vector3d(0, 0, 0), Vector3d(0, 0, 0), Material(Color(0, 0, 0)));
    }

    double lambda1 = (-b-sqrt(d))/(2*a);

    if (lambda1>=0) {
        return Hit(lambda1, ray.getLocation()+ray.getDirection()*lambda1, (v+Vector3d(0, 0, lambda1))/(v+Vector3d(0, 0, lambda1)).getLength(), this->getMaterial());
    }
    double lambda2 = (-b+sqrt(d))/(2*a);
    return Hit(lambda2, ray.getLocation()+ray.getDirection()*lambda2, (v+Vector3d(0, 0, lambda1))/(v+Vector3d(0, 0, lambda1)).getLength(), this->getMaterial());
}