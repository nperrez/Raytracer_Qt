//
// Created by Nicolas Perrez on 26/02/2026.
//

#include "Sphere.h"

#include <cmath>
#include <iostream>

Sphere::Sphere(Vector3d center, double radius, Color color) : center(center), radius(radius), color(color) {}

Vector3d Sphere::get_center() {
    return center;
}

double Sphere::get_radius() {
    return radius;
}

Color Sphere::get_color() {
    return color;
}

double Sphere::intersect(Ray ray) {
    double a = ray.getDirection()*ray.getDirection();
    Vector3d v = ray.getLocation() - center;
    double b = 2*(ray.getDirection()*v);
    double c = v*v - radius*radius;
    double d = b*b - 4*a*c;
    if (d < 0) {
        return -1;
    }
    double lambda1 = (-b-sqrt(d))/(2*a);
    //std::cout << "lambda1 = " << lambda1 << "\n";

    if (lambda1>=0) {
        return lambda1;
    } else {
        return (-b+sqrt(d))/(2*a);
    }

}