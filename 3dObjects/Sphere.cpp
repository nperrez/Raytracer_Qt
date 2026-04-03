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
    const Vector3d rayDirection = ray.getDirection().normalize();
    const double a = ray.getDirection()*ray.getDirection();
    const Vector3d v = ray.getLocation() - center;
    const double b = 2*(ray.getDirection()*v);
    const double c = v*v - radius*radius;
    const double d = b*b - 4*a*c;
    if (d < 0) {
        Hit hit2 = Hit(-1, Vector3d(0, 0, 0), Vector3d(0, 0, 0), Material(Color(0, 0, 0)));
        Vector3d normal = Vector3d(0, 0, 0);
        hit2.setFrontFace(rayDirection, normal);
        return hit2;
    }

    double lambda1 = (-b-sqrt(d))/(2*a);

    if (lambda1>=0) {
        Hit hit2 = Hit(lambda1, ray.getLocation()+ray.getDirection()*lambda1, (v+ray.getDirection() * lambda1)/(v+ray.getDirection()*lambda1).getLength(), this->getMaterial());
        Vector3d normal = (hit2.getPosition() - center).normalize();
        hit2.setFrontFace(rayDirection, normal);
        return hit2;
    }
    double lambda2 = (-b+sqrt(d))/(2*a);
    Hit hit2 = Hit(lambda2, ray.getLocation()+ray.getDirection()*lambda2, (v+ray.getDirection()*lambda2)/(v+ray.getDirection()*lambda2).getLength(), this->getMaterial());
    Vector3d normal = (hit2.getPosition() - center).normalize();
    hit2.setFrontFace(rayDirection, normal);
    return hit2;
}