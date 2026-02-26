//
// Created by Nicolas Perrez on 26/02/2026.
//

#ifndef RAYTRACER_QT_BALL_H
#define RAYTRACER_QT_BALL_H
#include "Color.h"
#include "Ray.h"
#include "Vector3d.h"


class Sphere {

private:
    Vector3d center;
    double radius;
    Color color;

public:
    Sphere(Vector3d center, double radius, Color color);

    Vector3d get_center();
    double get_radius();
    Color get_color();

    double intersect(Ray ray);
};


#endif //RAYTRACER_QT_BALL_H